#include "paths.h"
#include "pkg_file.h"
#include "platform.h"

#include <string.h>

#ifdef OS_WINDOWS
# include <Windows.h>
#endif


namespace Paths
{

char resourcePath[maxPath] = ".";

void SetResourcePath(const char *path)
{
	strncpy(resourcePath, path, maxPath - 1);
	resourcePath[maxPath - 1] = 0;
}


// mod support
enum {maxModPaths = 16};
static FixedString<maxPath> _modPath[maxModPaths];
static int _modPaths = 0;

void ResetModPaths()
{
	_modPaths = 0;
}

bool PushModPath(const FixedString<maxPath> &path)
{
	if (path.empty())
		return false;
	if (_modPaths >= maxModPaths)
		return false;

	_modPath[_modPaths] = path;
	_modPaths++;

	return true;
}

void PopModPath()
{
	if (_modPaths <= 0)
		return;

	_modPaths--;
}

bool ModFindFile(const FixedString<maxPath> &path, FixedString<512> &modPath, unsigned int *fileSize)
{
	for (int n = _modPaths - 1; n >= 0; n--)
	{
		modPath = FixedString<maxPath>(resourcePath) + "/" + _modPath[n] + "/" + path;

		unsigned int size = PkgFileSize(modPath.c_str());
		if (size > 0)
		{
			if (fileSize)
				*fileSize = size;

			return true;
		}
	}

	return false;
}

// OS paths
#if !defined(OS_MAC)
FixedString<maxPath> GetOSHomePath()
{
    return ".";
}
    
FixedString<maxPath> GetOSResourcePath()
{
    return ".";
}
#endif

void SetCurrentDirectoryToExeDirectory()
{
#ifdef OS_WINDOWS
	{
		char exePath[MAX_PATH + 1] = {};
		char *p = exePath + GetModuleFileName(GetModuleHandle(NULL), exePath, MAX_PATH);
		while (p != exePath && *p != '\\' && *p != '/') p--;
		*p = 0;
		SetCurrentDirectory(exePath);
	}
#endif
}

} // namespace Paths
