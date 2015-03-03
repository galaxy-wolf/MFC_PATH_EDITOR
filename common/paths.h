#ifndef PATHS_H
#define PATHS_H

#include "fixed_string.h"

namespace Paths
{

enum {maxPath = 512};

// base data path, mod system appends to this
extern char resourcePath[maxPath];
void SetResourcePath(const char *path);

// mod support
void ResetModPaths();
bool PushModPath(const FixedString<maxPath> &path);
void PopModPath();
bool ModFindFile(const FixedString<maxPath> &path, FixedString<maxPath> &modPath, unsigned int *fileSize = 0);

// OS paths
FixedString<maxPath> GetOSHomePath();
FixedString<maxPath> GetOSResourcePath();

// set the working directory to the executable's directory
void SetCurrentDirectoryToExeDirectory();
	
} // namespace Paths

#endif
