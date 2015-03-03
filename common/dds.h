#ifndef DDS_H
#define DDS_H

#include "texture_loader.h"

class DDS : public TextureLoader
{
public:

	DDS();

	bool load(const char *filename);
private:
};

#endif
