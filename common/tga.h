#ifndef TGA_H
#define TGA_H

#include "texture_loader.h"

class TGA : public TextureLoader
{
public:

	TGA();

	bool load(const char *filename);
private:
};

#endif