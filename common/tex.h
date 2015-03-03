#ifndef TEX_H
#define TEX_H

#include "texture_loader.h"

class TEX : public TextureLoader
{
public:

	TEX();

	bool load(const char *filename);
private:
};

#endif
