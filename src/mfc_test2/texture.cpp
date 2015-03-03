/***************************************************
To read a jpg image file and download
it as a texture map for openGL
Derived from Tom Lane's example.c
-- Obtain & install jpeg stuff from web
(jpeglib.h, jerror.h jmore.h, jconfig.h,jpeg.lib)
****************************************************/
#include "texture.h"
//将数据保存为bmp图像的函数



//================================
GLuint LoadJPEG(const char* FileName)
//================================
{
	unsigned long x, y;
	unsigned int texture_id;
	unsigned long data_size;     // length of the file
	int channels;               //  3 =>RGB   4 =>RGBA 
	unsigned int type;
	unsigned char * rowptr[1];  // pointer to an array
	unsigned char * jdata;      // data for the image
	unsigned char * rjdata;     // data for the image with reversed row
	struct jpeg_decompress_struct info; //for our jpeg info
	struct jpeg_error_mgr err;          //the error handler

	FILE* file = fopen(FileName, "rb");  //open the file

	info.err = jpeg_std_error(&err);
	jpeg_create_decompress(&info);   //fills info structure

	//if the jpeg file doesn't load
	if (!file) {
		fprintf(stderr, "Error reading JPEG file %s!", FileName);
		return -1;
	}

	jpeg_stdio_src(&info, file);
	jpeg_read_header(&info, TRUE);   // read jpeg file header

	jpeg_start_decompress(&info);    // decompress the file

	//set width and height
	x = info.output_width;
	y = info.output_height;
	channels = info.num_components;
	type = GL_RGB;
	if (channels == 4) type = GL_RGBA;

	data_size = x * y * 3;

	//--------------------------------------------
	// read scanlines one at a time & put bytes 
	//    in jdata[] array. Assumes an RGB image
	//--------------------------------------------
	jdata = (unsigned char *)malloc(data_size);
	while (info.output_scanline < info.output_height) // loop
	{
		// Enable jpeg_read_scanlines() to fill our jdata array
		rowptr[0] = (unsigned char *)jdata +  // secret to method
			3 * info.output_width * (info.output_height - info.output_scanline - 1);

		jpeg_read_scanlines(&info, rowptr, 1);
	}
	//---------------------------------------------------

	jpeg_finish_decompress(&info);   //finish decompressing

	char fileName_bmp[1024];
	sprintf(fileName_bmp, "%s", FileName);
	int len = strlen(FileName);
	while (fileName_bmp[--len] != '.');
	fileName_bmp[++len] = 'b';
	fileName_bmp[++len] = 'm';
	fileName_bmp[++len] = 'p';
	fileName_bmp[++len] = '\0';
	//SaveBMP1(fileName_bmp, jdata, x, y);

	GLint glMaxTexDim;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);
	//----- create OpenGL tex map (omit if not needed) --------
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	switch (channels)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
			0, GL_LUMINANCE, GL_UNSIGNED_BYTE, jdata);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
			0, GL_RGB, GL_UNSIGNED_BYTE, jdata);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
			0, GL_RGBA, GL_UNSIGNED_BYTE, jdata);
		break;
	default:
		fclose(file);
		free(jdata);
		glDeleteTextures(1, &texture_id);
		return -1;
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	Sleep(1000);
	fclose(file);                    //close the file
	free(jdata);

	return texture_id; // for OpenGL tex maps
}