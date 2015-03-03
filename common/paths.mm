#include "paths.h"


namespace Paths
{

FixedString<maxPath> GetOSHomePath()
{
	char pathBuffer[maxPath] = {};
    CFStringGetFileSystemRepresentation((__bridge CFStringRef)NSHomeDirectory(), pathBuffer, maxPath);
	pathBuffer[maxPath - 1] = 0;
	
	return pathBuffer;
}

FixedString<maxPath> GetOSResourcePath()
{
	char pathBuffer[maxPath] = {};
    CFStringGetFileSystemRepresentation((__bridge CFStringRef)[[NSBundle mainBundle] resourcePath], pathBuffer, maxPath);
	pathBuffer[maxPath - 1] = 0;
	
	return pathBuffer;
}

} // namespace Paths
