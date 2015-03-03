#include "psd.h"
#include "util.h"
#include "debug.h"
#include "pkg_file.h"
#include "byte_reader.h"

// matching pop at bottom
#pragma pack(2)

struct PSDHeader
{
	char signature[4];
	short version;
	char reserved[6];
	short channels;
	int rows;
	int columns;
	short depth;
	short mode;
};

#pragma pack()


PSD::PSD()
{
}

bool PSD::load(const char *filename)
{
	clear();

	unsigned int dataLen;
	unsigned char *filedata = PkgFileRead(filename, &dataLen);
	if (!filedata)
		return false;
	ByteReader reader(filedata, filedata + dataLen);

	bool ok = false;
	do
	{
		PSDHeader header;
		if (sizeof(PSDHeader) != reader.get(&header, sizeof(PSDHeader)))
			break;

		if (header.signature[0] != '8' || header.signature[1] != 'B' || header.signature[2] != 'P' || header.signature[3] != 'S')
			break;

		channels = REVERSE2(header.channels);
		if (channels > 4)
		{
			DebugPrint("too many channels: %s\n", filename);
			assert(0);
			channels = 4;
		}
		if (channels == 0)
			break;

		height = REVERSE4(header.rows);
		width = REVERSE4(header.columns);
		short depth = REVERSE2(header.depth);
		if (depth != 8)
			break;

		short mode = REVERSE2(header.mode);
		if (mode != 1 && mode != 3 && mode != 7) // grayscale, rgb, multichannel
			break;

		int temp;
		reader.get(&temp, 4);
		temp = REVERSE4(temp);
		reader.seek(temp, ByteReader::seek_cur); // color mode data (should be empty anyway)
		reader.get(&temp, 4);
		temp = REVERSE4(temp);
		reader.seek(temp, ByteReader::seek_cur); // image resources
		reader.get(&temp, 4);
		temp = REVERSE4(temp);
		reader.seek(temp, ByteReader::seek_cur); // misc (layer and mask info)

		short compression;
		reader.get(&compression, sizeof(short));
		compression = REVERSE2(compression);
		if (compression != 0 && compression != 1) // uncompressed or RLE only
			break;

		data = new unsigned char [channels * width * height];
		unsigned char *cdata = new unsigned char [width * height];
		if (compression == 0)
		{
			for (int c = 0; c < channels; c++)
			{
				reader.get(cdata, width * height);
				for (int n = 0; n < width * height; n++)
				{
					data[n * channels + c] = cdata[n];
				}
			}
		}
		else // RLE
		{
			int origchannels = REVERSE2(header.channels);
			int rowcount = origchannels * height; // use original (possibly > 4) channels
			short *linelen = new short [rowcount];
			reader.get(linelen, sizeof(short) * rowcount);
			int n;
			for (n = 0; n < rowcount; n++)
			{
				linelen[n] = REVERSE2(linelen[n]);
			}

			char *ctemp = new char [width + 127]; // maximum RLE size is original size + 127?
			for (int c = 0; c < channels; c++)
			{
				for (n = 0; n < height; n++) // read in up to first 4 channels
				{
					reader.get(ctemp, linelen[c * height + n]);
					util::RLEUnpack(ctemp, (char *)&cdata[n * width], linelen[c * height + n]);
				}
				for (n = 0; n < width * height; n++)
				{
					data[n * channels + c] = cdata[n];
				}
			}
			delete [] ctemp;
			delete [] linelen;
		}
		delete [] cdata;

		// swap r and b, we want bgr or bgra
		if (channels > 2)
		{
			unsigned char btemp;
			for (int n = 0; n < width * height; n++)
			{
				btemp = data[n * channels + 0];
				data[n * channels + 0] = data[n * channels + 2];
				data[n * channels + 2] = btemp;
			}
		}

		// no respectable texture format is complete unless it is stored upside down
		for (int h = 0; h < height / 2; h++)
		{
			for (int w = 0; w < width * channels; w++)
			{
				unsigned char temp = data[h * width * channels + w];
				data[h * width * channels + w] = data[(height - h - 1) * width * channels + w];
				data[(height - h - 1) * width * channels + w] = temp;
			}
		}

		ok = true;
	} while (0);

	delete [] filedata;

	return ok;
}
