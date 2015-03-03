#ifndef PACKAGE_H
#define PACKAGE_H

#include "types.h"

class Package
{
public:

	class FileHeader
	{
	public:

		enum {maxName = 32};
		enum
		{
			COMP_NONE = 0,
			COMP_LZMA,
		};

		unsigned int nextFile; // file position of next entry
		unsigned int compType;
		unsigned int uncompSize;
		char name[maxName];

		FileHeader()
		{
			zero();
		}

		void zero()
		{
			nextFile = 0;
			compType = 0;
			uncompSize = 0;
			for (int n = 0; n < maxName; n++)
				name[n] = 0;
		}
	};

	FileHeader currentFile;


	Package();
	~Package();

	void close();
	bool open(const char *filename, bool _write = false, bool _append = false);

	bool seekToFile(const char *name);
	bool firstFile();
	bool nextFile();

	bool readFile(unsigned char *buffer, unsigned int bufferLen);

	bool appendFile(const char *name, const unsigned char *buffer, unsigned int bufferLen, int compType = FileHeader::COMP_LZMA);
	bool appendFileFromFile(const char *name, const char *filename, int compType = FileHeader::COMP_LZMA);

	// props is 5 bytes
	// compression levels below 5 are 'fast' and not multithreaded
	// destLen for decompress must be the uncompressed size
	static bool LZMADecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen, const unsigned char *props);
	static bool LZMACompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen, unsigned char *outProps, int level = 5);

#ifdef ENABLE_ZLIB
	// destLen for decompress must be the uncompressed size
	static bool ZLibDecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen);
	static bool ZLibCompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen, int level = 5);
#endif

	static bool RLEDecompress(unsigned char *dest, size_t destLen, const unsigned char *src, size_t srcLen);
	static bool RLECompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen);

#ifdef ENABLE_ZLIB
	static bool RLEZLibDecompress(unsigned char *dest, unsigned char *destTemp, size_t destLen, size_t rleSize, const unsigned char *src, size_t srcLen);
	static bool RLEZLibCompress(unsigned char *dest, unsigned char *destTemp, size_t *destLen, size_t *rleSize, const unsigned char *src, size_t srcLen, int level = 5);
#endif

private:

	void *file;
	bool readyToReadFile;

	bool write;
	bool append;

	bool readFileCompNone(unsigned char *buffer, unsigned int bufferLen);
	bool readFileCompLZMA(unsigned char *buffer, unsigned int bufferLen);
};

#endif
