#ifndef PSD_H
#define PSD_H

#include "texture_loader.h"

class PSD : public TextureLoader
{
public:

	PSD();

	bool load(const char *filename);
private:
};

#endif