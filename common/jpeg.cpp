#include "platform.h"
#include "jpeg.h"
#include "paths.h"

#include <stdio.h>
#include <jpeglib.h>

#if defined(COMPILER_VS)
# define snprintf _snprintf
#endif


JPEG::JPEG()
{
}

bool JPEG::load(const char *filename)
{
	clear();

	char fullname[512];
	snprintf(fullname, 512 - 1, "%s/%s", Paths::resourcePath, filename);

	FILE *file;
	file = fopen(fullname, "rb");
	if (!file)
		return false;

	struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, true);

	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;
	channels = cinfo.output_components;
	data = new unsigned char [width * height * channels];

    while (cinfo.output_scanline < cinfo.output_height)
    {
        int currentScanline = cinfo.output_scanline;
		// read top to bottom
		JSAMPROW buffer = data + width * channels * (height - currentScanline - 1);

        jpeg_read_scanlines(&cinfo, &buffer, 1);

		if (channels == 3)
		{
			// rgb -> bgr
			unsigned char *end = data + width * channels * (height - currentScanline);
			unsigned char *p = data + width * channels * (height - currentScanline - 1);
			while (p != end)
			{
				unsigned char temp = *p;
				*p = *(p + 2);
				*(p + 2) = temp;
				p += 3;
			}
		}
    }
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

	fclose(file);
	return true;
}

bool JPEG::save(const char *filename, const unsigned char *src, int width, int height, int components, int quality)
{
	if (components != 1 && components != 3)
		return false;

	FILE *file = fopen(filename, "wb");
	if (!file)
		return false;

	struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, file);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = components;
	if (components == 3)
		cinfo.in_color_space = JCS_RGB;
	else
		cinfo.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, FALSE);

    jpeg_start_compress(&cinfo, TRUE);
	for (int n = 0; n < height; n++)
	{
		// write top to bottom
		JSAMPROW rowptr = (JSAMPROW)(src + width * components * (height - n - 1));
		jpeg_write_scanlines(&cinfo, (JSAMPARRAY)&rowptr, 1);
	}
    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);

	fclose(file);

	return true;
}