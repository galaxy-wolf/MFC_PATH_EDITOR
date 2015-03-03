#include "dds.h"
#include "pkg_file.h"
#include "byte_reader.h"
#include "debug.h"
#include "util.h"

// DDS texture loader, parts are based on public source code from John Watte's implementation
// http://www.mindcontrol.org/~hplus/graphics/dds-info/
// and parts are based on the old nv_dds.h/nv_dds.cpp from NVIDIA (this is a good place to look for Cube and 3D texture loading)

//  little-endian
#define DDS_MAGIC 0x20534444

//  DDS_header.dwFlags
#define DDSD_CAPS                   0x00000001 
#define DDSD_HEIGHT                 0x00000002 
#define DDSD_WIDTH                  0x00000004 
#define DDSD_PITCH                  0x00000008 
#define DDSD_PIXELFORMAT            0x00001000 
#define DDSD_MIPMAPCOUNT            0x00020000 
#define DDSD_LINEARSIZE             0x00080000 
#define DDSD_DEPTH                  0x00800000 

//  DDS_header.sPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 

//  DDS_header.sCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008 
#define DDSCAPS_TEXTURE             0x00001000 
#define DDSCAPS_MIPMAP              0x00400000 

//  DDS_header.sCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#define D3DFMT_DXT1     '1TXD'    //  DXT1 compression texture format 
#define D3DFMT_DXT3     '3TXD'    //  DXT3 compression texture format 
#define D3DFMT_DXT5     '5TXD'    //  DXT5 compression texture format 

union DDSHeader
{
	struct
	{
		unsigned int    dwMagic;
		unsigned int    dwSize;
		unsigned int    dwFlags;
		unsigned int    dwHeight;
		unsigned int    dwWidth;
		unsigned int    dwPitchOrLinearSize;
		unsigned int    dwDepth;
		unsigned int    dwMipMapCount;
		unsigned int    dwReserved1[11];

		//  DDPIXELFORMAT
		struct
		{
			unsigned int    dwSize;
			unsigned int    dwFlags;
			unsigned int    dwFourCC;
			unsigned int    dwRGBBitCount;
			unsigned int    dwRBitMask;
			unsigned int    dwGBitMask;
			unsigned int    dwBBitMask;
			unsigned int    dwAlphaBitMask;
		} sPixelFormat;

		//  DDCAPS2
		struct
		{
			unsigned int    dwCaps1;
			unsigned int    dwCaps2;
			unsigned int    dwDDSX;
			unsigned int    dwReserved;
		} sCaps;
		unsigned int dwReserved2;
	};

	char data[128];
};

struct DXTColBlock
{
    unsigned short col0;
    unsigned short col1;

    unsigned char row[4];
};

struct DXT3AlphaBlock
{
    unsigned short row[4];
};

struct DXT5AlphaBlock
{
    unsigned char alpha0;
    unsigned char alpha1;
        
    unsigned char row[6];
};

static void FlipDXT1Block(unsigned char *data)
{
	DXTColBlock *colBlock = (DXTColBlock*)data;

	util::swap(colBlock->row[0], colBlock->row[3]);
	util::swap(colBlock->row[1], colBlock->row[2]);
}

static void FlipDXT3Block(unsigned char *data)
{
	DXT3AlphaBlock *alphaBlock = (DXT3AlphaBlock*)data;

	util::swap(alphaBlock->row[0], alphaBlock->row[3]);
    util::swap(alphaBlock->row[1], alphaBlock->row[2]);

	FlipDXT1Block(data + 8);
}

static void FlipDXT5Block(unsigned char *data)
{
	DXT5AlphaBlock *alphaBlock = (DXT5AlphaBlock*)data;

	// DXT5 alpha block line swapping from: http://www.racer.nl/tech/dds.htm
	// 3 bits of indexing per pixel
	// 12 bits per line
	// 48 bits total
	unsigned int line_0_1 = alphaBlock->row[0] + 256 * (alphaBlock->row[1] + 256 * alphaBlock->row[2]);
	unsigned int line_2_3 = alphaBlock->row[3] + 256 * (alphaBlock->row[4] + 256 * alphaBlock->row[5]);
	// swap lines 0 and 1 in line_0_1
	unsigned int line_1_0 = ((line_0_1 & 0x000fff) << 12) |	((line_0_1 & 0xfff000) >> 12);
	// swap lines 2 and 3 in line_2_3
	unsigned int line_3_2 = ((line_2_3 & 0x000fff) << 12) |	((line_2_3 & 0xfff000) >> 12);
	alphaBlock->row[0] = line_3_2 & 0xff;
	alphaBlock->row[1] = (line_3_2 & 0xff00) >> 8;
	alphaBlock->row[2] = (line_3_2 & 0xff0000) >> 16;
	alphaBlock->row[3] = line_1_0 & 0xff;
	alphaBlock->row[4] = (line_1_0 & 0xff00) >> 8;
	alphaBlock->row[5] = (line_1_0 & 0xff0000) >> 16;

	FlipDXT1Block(data + 8);
}

DDS::DDS()
{
}

bool DDS::load(const char *filename)
{
	clear();

	unsigned int dataLen;
	unsigned char *filedata = PkgFileRead(filename, &dataLen);
	if (!filedata)
		return false;
	ByteReader reader(filedata, filedata + dataLen);

	bool ok = false;

	DDSHeader hdr;
	if (sizeof(DDSHeader) != reader.get(&hdr, sizeof(DDSHeader)))
		goto failure;
	assert(hdr.dwMagic == DDS_MAGIC);
	assert(hdr.dwSize == 124);
	int totalSize = dataLen - sizeof(DDSHeader);

	if (hdr.dwMagic != DDS_MAGIC || hdr.dwSize != 124 ||
		!(hdr.dwFlags & DDSD_PIXELFORMAT) || !(hdr.dwFlags & DDSD_CAPS))
	{
		goto failure;
	}

	width = hdr.dwWidth;
	height = hdr.dwHeight;

	int blockSize = 0;
	void (*flipFunc)(unsigned char*) = 0;
	if ((hdr.sPixelFormat.dwFlags & DDPF_FOURCC) && hdr.sPixelFormat.dwFourCC == D3DFMT_DXT1)
	{
		blockSize = 8;
		flipFunc = FlipDXT1Block;
		if (hdr.sPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
		{
			channels = 4;
			compressedFormat = cf_dxt1a;
		}
		else
		{
			channels = 3;
			compressedFormat = cf_dxt1;
		}
	}
	else if ((hdr.sPixelFormat.dwFlags & DDPF_FOURCC) && hdr.sPixelFormat.dwFourCC == D3DFMT_DXT3)
	{
		blockSize = 16;
		flipFunc = FlipDXT3Block;
		channels = 4;
		compressedFormat = cf_dxt3;
	}
	else if ((hdr.sPixelFormat.dwFlags & DDPF_FOURCC) && hdr.sPixelFormat.dwFourCC == D3DFMT_DXT5)
	{
		blockSize = 16;
		flipFunc = FlipDXT5Block;
		channels = 4;
		compressedFormat = cf_dxt5;
	}
	else
	{
		goto failure;
	}

	assert(hdr.dwFlags & DDSD_LINEARSIZE);
	data = new unsigned char [totalSize];
	unsigned char *readPtr = data;

	int x = width;
	int y = height;
	mipmapCount = (hdr.dwFlags & DDSD_MIPMAPCOUNT) ? hdr.dwMipMapCount : 1;
	for (int n = 0; n < mipmapCount; n++)
	{
		int size = ((x + 3) / 4) * ((y + 3) / 4) * blockSize;
		if (n == 0)
		{
			assert(size == hdr.dwPitchOrLinearSize);
		}

		reader.get(readPtr, size);

		// flip vertically
		int blockXCount = x / 4;
		int blockYCount = y / 4;
		int blockRowSize = blockXCount * blockSize;
		for (int h = 0; h < blockYCount / 2; h++)
		{
			unsigned char *blockRow0 = readPtr + h * (x / 4) * blockSize;
			unsigned char *blockRow1 = readPtr + (blockYCount - h - 1) * (x / 4) * blockSize;
			// swap the rows
			for (int i = 0; i < blockRowSize / 4; i++)
			{
				util::swap<int>(((int*)blockRow0)[i], ((int*)blockRow1)[i]);
			}

			// flip vertically inside the DXT blocks
			for (int w = 0; w < blockXCount; w++)
			{
				unsigned char *block0 = blockRow0 + w * blockSize;
				unsigned char *block1 = blockRow1 + w * blockSize;

				flipFunc(block0);
				flipFunc(block1);
			}
		}

		readPtr += size;
		x = (x + 1) >> 1;
		y = (y + 1) >> 1;
	}
	assert(readPtr == data + totalSize);

	ok = true;

failure:

	delete [] filedata;

	return ok;
}
