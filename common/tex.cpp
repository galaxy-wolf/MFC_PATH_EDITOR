#include "tex.h"
#include "pkg_file.h"
#include "byte_reader.h"


TEX::TEX()
{
}

bool TEX::load(const char *filename)
{
	clear();

	unsigned int dataLen;
	unsigned char *filedata = PkgFileRead(filename, &dataLen);
	if (!filedata)
		return false;
	ByteReader reader(filedata, filedata + dataLen);

	reader.get(&width, sizeof(int));
	reader.get(&height, sizeof(int));
	reader.get(&channels, sizeof(int));

	data = new unsigned char [width * height * channels];
	reader.get(data, width * height * channels);

	delete [] filedata;

	return true;
}
