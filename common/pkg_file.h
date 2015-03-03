#ifndef PKG_FILE_H
#define PKG_FILE_H

class ScratchAllocator;

// if allocator == 0 it will use new to allocate
unsigned char* PkgFileRead(const char *path, unsigned int *dataLen, ScratchAllocator *allocator = 0);
// returns 0 if doesn't exist
unsigned int PkgFileSize(const char *path);

#endif
