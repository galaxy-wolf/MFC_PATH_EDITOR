#ifndef JPEG_H
#define JPEG_H

#include "texture_loader.h"

class JPEG : public TextureLoader
{
public:

	JPEG();

	// does not support loading from packages yet
	bool load(const char *filename);

	// quality 0 is worst, 100 best
	// handles grayscale and RGB, for now
	// JPEG compression can potentially work for any number of components
	// see libjpeg.doc special color spaces section
	static bool save(const char *filename, const unsigned char *src, int width, int height, int components, int quality);

private:
};

#endif
