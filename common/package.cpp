#include "package.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <LzmaLib.h>
#ifdef ENABLE_ZLIB
# include <zlib.h>
#endif

struct CompNoneHeader
{
};

struct CompLZMAHeader
{
	// explicit padding
	union
	{
		struct
		{
			unsigned int compSize;
			unsigned char props[LZMA_PROPS_SIZE];
		};
		struct
		{
			unsigned char bytes[16];
		};
	};
};


Package::Package()
{
	file = 0;
	readyToReadFile = false;

	write = false;
	append = false;
}

Package::~Package()
{
	close();
}

void Package::close()
{
	currentFile.zero();

	if (file)
		fclose((FILE*)file);
	file = 0;

	readyToReadFile = false;

	write = false;
	append = false;
}

bool Package::open(const char *filename, bool _write, bool _append)
{
	close();

	write = _write;
	append = _append;

	if (!write)
	{
		file = fopen(filename, "rb");
	}
	else
	{
		// need read access to update last record
		if (!append)
		{
			file = fopen(filename, "w+b"); // truncate
		}
		else
		{
			file = fopen(filename, "r+b"); // append
		}
	}
	if (!file)
		return false;

	return true;
}

bool Package::firstFile()
{
	readyToReadFile = false;

	if (0 != fseek((FILE*)file, 0, SEEK_SET))
		return false;
	currentFile.zero();
	if (1 != fread(&currentFile, sizeof(FileHeader), 1, (FILE*)file))
		return false;

	readyToReadFile = true;

	return true;
}

bool Package::nextFile()
{
	if (currentFile.nextFile == 0) // no next file
		return false;

	readyToReadFile = false;

	if (0 != fseek((FILE*)file, currentFile.nextFile, SEEK_SET))
		return false;
	currentFile.zero();
	if (1 != fread(&currentFile, sizeof(FileHeader), 1, (FILE*)file))
		return false;

	readyToReadFile = true;

	return true;
}

bool Package::seekToFile(const char *name)
{
	if (!file)
		return 0;
	if (!name)
		return 0;

	if (!firstFile())
		return false;

	while (strncmp(name, currentFile.name, FileHeader::maxName) != 0)
	{
		if (!nextFile())
			return false;
	}

	return true;
}

bool Package::readFile(unsigned char *buffer, unsigned int bufferLen)
{
	if (write)
		return false;
	if (!file)
		return false;
	if (!buffer)
		return false;
	if (!readyToReadFile)
		return false;
	if (bufferLen < currentFile.uncompSize)
		return false;

	readyToReadFile = false;

	switch (currentFile.compType)
	{
	case FileHeader::COMP_NONE:
		return readFileCompNone(buffer, bufferLen);
		break;

	case FileHeader::COMP_LZMA:
		return readFileCompLZMA(buffer, bufferLen);
		break;

	default:
		return false;
		break;
	}
}

bool Package::readFileCompNone(unsigned char *buffer, unsigned int bufferLen)
{
	if (1 != fread(buffer, currentFile.uncompSize, 1, (FILE*)file))
		return false;

	return true;
}

bool Package::readFileCompLZMA(unsigned char *buffer, unsigned int bufferLen)
{
	CompLZMAHeader compHeader;
	if (1 != fread(&compHeader, sizeof(compHeader), 1, (FILE*)file))
		return false;

	size_t srcLen = compHeader.compSize;
	unsigned char *src = (unsigned char*)malloc(srcLen);
	if (!src)
		return false;
	if (1 != fread(src, srcLen, 1, (FILE*)file))
	{
		free(src);
		return false;
	}

	if (!LZMADecompress(buffer, bufferLen, src, srcLen, compHeader.props))
	{
		free(src);
		return false;
	}

	free(src);
	return true;
}

bool Package::appendFileFromFile(const char *name, const char *filename, int compType)
{
	if (!filename)
		return false;

	FILE *input = fopen(filename, "rb");
	if (!input)
		return false;

	if (0 != fseek(input, 0, SEEK_END))
		return false;
	unsigned int filelen = (unsigned int)ftell(input);
	if (0 != fseek(input, 0, SEEK_SET))
		return false;

	bool rval = false;
	unsigned char *src = (unsigned char*)malloc(filelen);
	if (src)
	{
		if (1 == fread(src, filelen, 1, input))
		{
			if (appendFile(name, src, filelen))
				rval = true;
		}
		free(src);
	}
	fclose(input);

	return rval;
}

bool Package::appendFile(const char *name, const unsigned char *buffer, unsigned int bufferLen, int compType)
{
	if (!file)
		return false;
	if (!buffer)
		return false;
	if (bufferLen == 0)
		return false;
	if (!name)
		return false;

	if (0 != fseek((FILE*)file, 0, SEEK_END))
		return false;

	unsigned int offset = (unsigned int)ftell((FILE*)file);

	FileHeader fh;
	fh.nextFile = 0;
	fh.compType = compType;
	fh.uncompSize = bufferLen;
	strncpy(fh.name, name, FileHeader::maxName);

	// try LZMA compression, if requested (the default)
	// if it fails (usually because it compressed larger than the input data)
	// fall-through and write it uncompressed
	if (fh.compType == FileHeader::COMP_LZMA)
	{
		CompLZMAHeader compLZMAHeader;
		size_t compSize = bufferLen; // compressed results must be <= to this value, otherwise we store uncompressed
		unsigned char *dest = (unsigned char*)malloc(compSize);
		if (!dest)
			return false;
		if (LZMACompress(dest, &compSize, buffer, bufferLen, compLZMAHeader.props))
		{
			compLZMAHeader.compSize = (unsigned int)compSize;

			if (1 != fwrite(&fh, sizeof(FileHeader), 1, (FILE*)file))
			{
				free(dest);
				return false;
			}
			if (1 != fwrite(&compLZMAHeader, sizeof(CompLZMAHeader), 1, (FILE*)file))
			{
				free(dest);
				return false;
			}
			if (1 != fwrite(dest, compLZMAHeader.compSize, 1, (FILE*)file))
			{
				free(dest);
				return false;
			}
		}
		else
		{
			// write uncompressed instead
			fh.compType = FileHeader::COMP_NONE;
		}
		free(dest);
	}
	// fall-through from LZMA if uncompressed is better
	if (fh.compType == FileHeader::COMP_NONE)
	{
		if (1 != fwrite(&fh, sizeof(FileHeader), 1, (FILE*)file))
		{
			return false;
		}
		// nothing in CompNoneHeader yet
		if (1 != fwrite(buffer, bufferLen, 1, (FILE*)file))
		{
			return false;
		}
	}

	// update the last record (if it exists) to point to the new file
	if (firstFile())
	{
		while (nextFile());
		if (currentFile.nextFile == 0)
		{
			currentFile.nextFile = offset;
			// rewrite the file record
			if (0 != fseek((FILE*)file, -(long)sizeof(FileHeader), SEEK_CUR))
			{
				return false;
			}
			if (1 != fwrite(&currentFile, sizeof(FileHeader), 1, (FILE*)file))
			{
				return false;
			}
		}
		else
		{
			// shouldn't already point anywhere
			return false;
		}
	}

	return true;
}

bool Package::LZMADecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen, const unsigned char *props)
{
	int error = LzmaUncompress(dest, &destLen, src, &srcLen, props, LZMA_PROPS_SIZE);
	if (error)
		return false;
	return true;
}

bool Package::LZMACompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen, unsigned char *outProps, int level)
{
	size_t outPropsSize = LZMA_PROPS_SIZE;
	int error = LzmaCompress(dest, destLen, src, srcLen, outProps, &outPropsSize, level, 0, -1, -1, -1, -1, -1);
	if (error)
	{
		// write uncompressed, it probably compressed larger than the source data
		return false;
	}
	return true;
}

#ifdef ENABLE_ZLIB
bool Package::ZLibDecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen)
{
	bool rval = false;

	z_stream zstr;
	memset(&zstr, 0, sizeof(z_stream));
	inflateInit(&zstr);

	zstr.next_in = (Bytef*)src;
	zstr.avail_in = (unsigned int)srcLen;
	zstr.next_out = (Bytef*)dest;
	zstr.avail_out = (unsigned int)destLen;
	if (Z_STREAM_END == inflate(&zstr, Z_FINISH))
		rval = true;

	inflateEnd(&zstr);

	return rval;
}

bool Package::ZLibCompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen, int level)
{
	// see deflateBound() for getting a size estimate
	bool rval = false;

	z_stream zstr;
	memset(&zstr, 0, sizeof(z_stream));
	deflateInit2(&zstr, level, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
	zstr.next_in = (Bytef*)src;
	zstr.avail_in = (unsigned int)srcLen;
	zstr.next_out = (Bytef*)dest;
	zstr.avail_out = (unsigned int)*destLen;
	if (Z_STREAM_END == deflate(&zstr, Z_FINISH))
		rval = true;

	*destLen = zstr.total_out;
	deflateEnd(&zstr);

	return rval;
}
#endif

bool Package::RLEDecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen)
{
	const unsigned char *in = src;
	const unsigned char *inEnd = in + srcLen;
	unsigned char *out = dest;
	unsigned char *outEnd = out + destLen;

	while (in < inEnd)
	{
		if (*in == 0)
		{
			in++;
			if (in >= inEnd)
				return false;

			int count = *in++;
			for (int n = 0; n < count; n++)
			{
				if (in >= inEnd || out >= outEnd)
					return false;

				*out++ = *in++;
			}
		}
		else
		{
			int count = *in++;
			if (in >= inEnd)
				return false;

			unsigned char value = *in++;
			for (int n = 0; n < count; n++)
			{
				if (out >= outEnd)
					return false;

				*out++ = value;
			}
		}
	}

	return true;
}

bool Package::RLECompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen)
{
	const unsigned char *in = src;
	const unsigned char *inEnd = in + srcLen;
	unsigned char *out = dest;
	unsigned char *outEnd = out + *destLen;

	bool uncompressed = false;
	unsigned char *uncompressedCounter = 0;
	while (in < inEnd)
	{
		unsigned char value = *in;

		int count = 1;
		const unsigned char *next = in + 1;
		while (next < inEnd && *next++ == value) count++;

		if (count > 2)
		{
			uncompressed = false;

			if (out + 2 > outEnd)
				return false; // out of room

			if (count > 255)
				count = 255;
			*out++ = count;
			*out++ = value;
		}
		else
		{
			count = 1;

			if (!uncompressed)
			{
				if (out + 2 > outEnd)
					return false; // out of room
				*out++ = 0;
				uncompressedCounter = out++;
				*uncompressedCounter = 0;
			}
			uncompressed = true;

			if (out + 1 > outEnd)
				return false; // out of room
			*out++ = value;

			(*uncompressedCounter)++;
			if (*uncompressedCounter == 255)
				uncompressed = false;
		}

		in += count;
	}

	*destLen = out - dest;

	return true;
}

#ifdef ENABLE_ZLIB
bool Package::RLEZLibDecompress(unsigned char *dest, unsigned char *destTemp, size_t destLen, size_t rleSize, const unsigned char *src, size_t srcLen)
{
	if (!ZLibDecompress(destTemp, rleSize, src, srcLen))
		return false;
	if (!RLEDecompress(dest, destLen, destTemp, rleSize))
		return false;
	return true;
}

bool Package::RLEZLibCompress(unsigned char *dest, unsigned char *destTemp, size_t *destLen, size_t *rleSize, const unsigned char *src, size_t srcLen, int level)
{
	if (!RLECompress(destTemp, rleSize, src, srcLen))
		return false;
	if (!ZLibCompress(dest, destLen, destTemp, *rleSize, level))
		return false;
	return true;
}
#endif
