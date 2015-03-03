#include "platform.h"
#include "pkg_file.h"
#include "package.h"
#include "allocator.h"
#include "types.h"
#include "paths.h"

#include <stdio.h>
#include <string.h>

#if defined(COMPILER_VS)
# define snprintf _snprintf
#endif


unsigned char* PkgFileRead(const char *dir, const char *name, unsigned int *dataLen, ScratchAllocator *allocator)
{
	if (!dir)
		return 0;
	if (!name)
		return 0;

	unsigned int size = 0;
	unsigned char *data = 0;

	char filename[512];
	sprintf(filename, "%s/%s", dir, name);

	FILE *file = fopen(filename, "rb");
	if (file)
	{
		// unpacked file
		fseek(file, 0, SEEK_END);
		size = (unsigned int)ftell(file);
		fseek(file, 0, SEEK_SET);

		if (allocator)
		{
			data = allocator->allocate(size);
		}
		else
		{
			data = new unsigned char [size];
		}

		if (data)
		{
			fread(data, size, 1, file);
		}

		fclose(file);
	}
	else
	{
		// packed file
		char packagename[512];
		sprintf(packagename, "%s/package.pkg", dir);

		Package package;
		if (package.open(packagename))
		{
			if (package.seekToFile(name))
			{
				size = package.currentFile.uncompSize;

				if (allocator)
				{
					data = allocator->allocate(size);
				}
				else
				{
					data = new unsigned char [size];
				}

				if (data)
				{
					package.readFile(data, size);
				}
			}
			package.close();
		}
	}

	if (dataLen)
		*dataLen = size;

	return data;
}

unsigned int PkgFileSize(const char *dir, const char *name)
{
	if (!dir)
		return 0;
	if (!name)
		return 0;

	unsigned int size = 0;

	char filename[512];
	sprintf(filename, "%s/%s", dir, name);

	FILE *file = fopen(filename, "rb");
	if (file)
	{
		// unpacked file
		fseek(file, 0, SEEK_END);
		size = (unsigned int)ftell(file);
		fseek(file, 0, SEEK_SET);

		fclose(file);
	}
	else
	{
		// packed file
		char packagename[512];
		sprintf(packagename, "%s/package.pkg", dir);

		Package package;
		if (package.open(packagename))
		{
			if (package.seekToFile(name))
			{
				size = package.currentFile.uncompSize;
			}
			package.close();
		}
	}

	return size;
}

unsigned char* PkgFileRead(const char *path, unsigned int *dataLen, ScratchAllocator *allocator)
{
	if (!path)
		return 0;

	char dir[512];
	char name[128];
	const char *p = strrchr(path, '/');
	if (p)
	{
		strncpy(dir, path, p - path);
		dir[p - path] = 0;
		strncpy(name, p + 1, 128);
		return PkgFileRead(dir, name, dataLen, allocator);
	}
	else
	{
		// no directories in the path
		dir[0] = '.';
		dir[1] = 0;
		return PkgFileRead(dir, path, dataLen, allocator);
	}
}

unsigned int PkgFileSize(const char *path)
{
	if (!path)
		return 0;

	char dir[512];
	char name[128];
	const char *p = strrchr(path, '/');
	if (p)
	{
		strncpy(dir, path, p - path);
		dir[p - path] = 0;
		strncpy(name, p + 1, 128);
		return PkgFileSize(dir, name);
	}
	else
	{
		// no directories in the path
		dir[0] = '.';
		dir[1] = 0;
		return PkgFileSize(dir, path);
	}
}
