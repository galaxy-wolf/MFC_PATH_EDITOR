#include "texture_loader.h"
#include "tex.h"
#include "psd.h"
#include "tga.h"
#include "jpeg.h"
#include "dds.h"
#include "paths.h"


TextureLoader::TextureLoader()
	: data(0)
{
	clear();
}

TextureLoader::~TextureLoader()
{
	clear();
}

void TextureLoader::clear()
{
	delete [] data;
	data = 0;

	channels = 0;

	mipmapCount = 1;

	compressedFormat = cf_none;
}

TextureLoader* TextureLoader::halve() const
{
	if (!data || width <= 1 || height <= 1 || compressedFormat != cf_none)
		return 0;

	int newWidth = width / 2;
	int newHeight = height / 2;

	TextureLoader *halved = new TEX; // create one of the simpler texture containers
	halved->width = newWidth;
	halved->height = newHeight;
	halved->channels = channels;
	halved->data = new unsigned char [halved->width * halved->height * halved->channels];

	unsigned char *dataDst = halved->data;

	for (int h = 0; h < newHeight; h++)
	{
		for (int w = 0; w < newWidth; w++)
		{
			for (int c = 0; c < channels; c++)
			{
				dataDst[(h * newWidth + w) * channels + c] = 
					(	data[(h * 2 * width + w * 2) * channels + c]
					+	data[(h * 2 * width + w * 2 + 1) * channels + c]
					+	data[((h * 2 + 1) * width + w * 2) * channels + c]
					+	data[((h * 2 + 1) * width + w * 2 + 1) * channels + c]) / 4;
			}
		}
	}

	return halved;
}

TextureLoader* TextureLoader::createFromFile(const char *name)
{
	TextureLoader *texData = 0;
	FixedString<Paths::maxPath> fullname;

	texData = new DDS;
	if (Paths::ModFindFile(FixedString<Paths::maxPath>("textures/") + name + ".dds", fullname)
		&& texData->load(fullname.c_str()))
	{
		return texData;
	}
	delete texData;

	texData = new TEX;
	if (Paths::ModFindFile(FixedString<Paths::maxPath>("textures/") + name + ".tex", fullname)
		&& texData->load(fullname.c_str()))
	{
		return texData;
	}
	delete texData;

	texData = new JPEG;
	if (Paths::ModFindFile(FixedString<Paths::maxPath>("textures/") + name + ".jpg", fullname)
		&& texData->load(fullname.c_str()))
	{
		return texData;
	}
	delete texData;

	texData = new PSD;
	if (Paths::ModFindFile(FixedString<Paths::maxPath>("textures/") + name + ".psd", fullname)
		&& texData->load(fullname.c_str()))
	{
		return texData;
	}
	delete texData;

	texData = new TGA;
	if (Paths::ModFindFile(FixedString<Paths::maxPath>("textures/") + name + ".tga", fullname)
		&& texData->load(fullname.c_str()))
	{
		return texData;
	}
	delete texData;

	return 0;
}
