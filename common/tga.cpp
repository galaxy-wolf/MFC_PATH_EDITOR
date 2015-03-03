#include "tga.h"
#include "pkg_file.h"
#include "byte_reader.h"

// matching pop at bottom
#pragma pack(1)

struct TGAHeader
{
    unsigned char	idLength;			// size of ID field
    unsigned char	colormapType;		// 1 = has a palette
    unsigned char	imageType;			// 0 = no image, 1 = palette, 2 = true-color, 3 = grayscale
										// 9 = RLE palette, 10 = RLE true-color, 11 = RLE grayscale

    short			colorMapStartIndex;	// index of first entry stored from palette
    short			colorMapLength;		// number of palette entries
    unsigned char	colorMapBits;		// size of each palette entry, 15, 16, 24 or 32 typically

    short			xOrigin;
    short			yOrigin;
    short			width;
    short			height;
    unsigned char	bitsPerPixel;
    unsigned char	flags;				// bits 0-3: alpha/attribute bits per pixel
										// bit 4: 0 = left origin, 1 = right origin
										// bit 5: 0 = bottom origin, 1 = top origin
};

#pragma pack()


TGA::TGA()
{
}

bool TGA::load(const char *filename)
{
	if (data)
		clear();

	unsigned int dataLen;
	unsigned char *filedata = PkgFileRead(filename, &dataLen);
	if (!filedata)
		return false;
	ByteReader reader(filedata, filedata + dataLen);

	bool ok = false;
	do
	{
		TGAHeader header;
		if (sizeof(TGAHeader) != reader.get(&header, sizeof(TGAHeader)))
			break;

		if (header.imageType != 2 && header.imageType != 3)
		{
			// only supporting non-RLE true-color and greyscale
			break;
		}
		if (header.flags & (1 << 4))
		{
			// flip horizontal
			break;
		}
		bool flipVertical = false;
		if (header.flags & (1 << 5))
		{
			// flip vertical
			flipVertical = true;
		}

		// skip the id and colormap sections (usually empty)
		reader.seek(header.idLength + header.colorMapLength * ((header.colorMapBits + 7) / 8), ByteReader::seek_cur);

		width = header.width;
		height = header.height;
		channels = (header.bitsPerPixel + 7) / 8;
		data = new unsigned char [width * height * channels];

		reader.get(data, width * height * channels);

		if (flipVertical)
		{
			for (int h = 0; h < height / 2; h++)
			{
				for (int w = 0; w < width * channels; w++)
				{
					unsigned char temp = data[h * width * channels + w];
					data[h * width * channels + w] = data[(height - h - 1) * width * channels + w];
					data[(height - h - 1) * width * channels + w] = temp;
				}
			}
		}

		ok = true;
	} while (0);

	delete [] filedata;

	return ok;
}
