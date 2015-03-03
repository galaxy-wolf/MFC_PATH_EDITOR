#ifndef DEBUG_MEMORY_H
#define DEBUG_MEMORY_H

#include "platform.h"

#if defined(MEMORY_TRACK_ENABLED) && defined(_DEBUG)

# ifdef COMPILER_VS

// enable file/line tracking in crt debug alloc functions
//#define _CRTDBG_MAP_ALLOC

void MemoryTrackInit();

# else

#  define MemoryTrackInit()

# endif

#else

# define MemoryTrackInit()

#endif

#endif
