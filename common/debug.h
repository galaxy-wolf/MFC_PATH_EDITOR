#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>


// memory tracking
#include "debug_memory.h"

#ifdef LOG_ENABLED
# define DebugPrint(...) _DebugPrint(__VA_ARGS__)
  void _DebugPrint(const char *format, ...);
# define DebugLog(...) _DebugLog(__VA_ARGS__)
  void _DebugLog(const char *format, ...);
#else
# define DebugPrint(...)
# define DebugLog(...)
#endif // LOG_ENABLED

#endif // DEBUG_H
