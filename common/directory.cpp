#include "directory.h"
#include "platform.h"
#include "file_constants.h"

#include <sys/stat.h>
#ifdef OS_WINDOWS
# include <windows.h>
# include <direct.h>
# include <io.h>
#else
# include <sys/types.h>
# include <dirent.h>
# include <dirent.h>
# define _mkdir(x) mkdir((x), (S_IRWXU | S_IRWXG | S_IRWXO))
#endif


namespace Directory
{

bool GetFileList(const std::string &dirPath, std::vector<std::string> &fileList, bool returnShortNames)
{
	std::string prefix;
	if (returnShortNames)
		prefix = "";
	else
		prefix = dirPath + '/';

#ifdef OS_WINDOWS
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	hFind = FindFirstFile((dirPath + "/*.*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		// no files found
	}
	else
	{
		do
		{
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			fileList.push_back(prefix + findFileData.cFileName);
		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
#else
	DIR *dir = opendir(dirPath.c_str());
	if (dir)
	{
		dirent *ent = readdir(dir);
		while (ent)
		{
			fileList.push_back(prefix + ent->d_name);

			ent = readdir(dir);
		}
		closedir(dir);
	}
#endif

	return true;
}

bool Create(const std::string &path)
{
	size_t srcPos = 0;

	char dirPath[FileConstants::maxPath];
	char *q = dirPath;

	while (srcPos < path.length() && (q - dirPath < FileConstants::maxPath - 1))
	{
		*q = path[srcPos];

		if ((*q == '/') || (*q == '\\'))
		{
			*(q + 0) = '/';
			*(q + 1) = '\0';

			_mkdir(dirPath);
		}

		srcPos++;
		q++;
	}

	*q = '\0';
	_mkdir(dirPath);

#ifdef WIN32
	_chmod(dirPath, _S_IREAD | _S_IWRITE);
#else
	chmod(dirPath, ACCESSPERMS);
#endif

	// check that the directory exists
#ifdef _WIN32
	_finddata_t fData;
	intptr_t hFind;
	hFind = _findfirst(dirPath, &fData);
	if (-1 == hFind)
		return false;
	_findclose(hFind);
#else
	DIR *dirCheck = opendir(dirPath);
	if (!dirCheck)
		return false;
	closedir(dirCheck);
#endif

	return true;
}

} // namespace Directory
