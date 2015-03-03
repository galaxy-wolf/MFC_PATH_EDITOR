#pragma once

#include <vector>
#include <string>


namespace Directory
{

// appends dirPath + short file name to fileList for each file found
// (note that fileList is not cleared or reset, only appended to)
bool GetFileList(const std::string &dirPath, std::vector<std::string> &fileList, bool returnShortNames);

// returns true if the directory was created or already exists
bool Create(const std::string &path);

} // namespace Directory
