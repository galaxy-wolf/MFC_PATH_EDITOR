#ifndef _CTEXTURES_H_
#define _CTEXTURES_H_

/*********************************************************************************************\
|	
|	Mark M. Lanning
|	Starlight Software Co.
|	
|	TextureObjects.h
|		these is the header for the TextureObjects classes that work with glaux to load bmp textures
|
\*********************************************************************************************/
const bool TEXTURE_DEBUG = true;			//set debug so we get feedback about what is going on
#define TEXT_DIR "data\\textures\\"
#pragma comment(lib, "glaux.lib")
#include <GL\glaux.h>

//#pragma comment(lib, "jpeg.lib")
//#include "jpeglib.h"


//Define a new structure to hold the image information
typedef struct _ImageDataRec {
	int rowSpan;
	int sizeX;
	int sizeY;
	unsigned char *data;
} ImageDataRec;


class CTextures
{
	public:

		// Typical constructor
		CTextures();

		// This will clean up memory and close the sound system
		~CTextures();

		// This will load the texture from the file and put it into the passed texture
		bool LoadTexture(char *mFileName, UINT &mTexture);

	private:
		// This checks to make sure the file exists
		bool FileExist(char *mFileName);
				
};

#endif
