#include "platform.h"
#include "debug.h"

#if defined(MEMORY_TRACK_ENABLED) && defined (_DEBUG)

#ifdef COMPILER_VS
#include <crtdbg.h>

unsigned __int64 totalAllocated = 0;
unsigned __int64 currentAllocated = 0;

int AllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine)
{
	// ignore crt allocs because we might be calling into the crt from this function
	if (nBlockUse == _CRT_BLOCK)
		return true;

	switch (nAllocType)
	{
	case _HOOK_ALLOC:
		totalAllocated += (unsigned __int64)nSize;
		currentAllocated += (unsigned __int64)nSize;
	case _HOOK_REALLOC:
		if (pvData)
			currentAllocated -= (unsigned __int64)_msize_dbg(pvData, nBlockUse);

		DebugPrint("alloc %d:\t%d bytes\ttotal: %.1f\tcurrent: %I64i\n", lRequest, nSize, (int)(totalAllocated / 1024) / 1024.0f, currentAllocated);
		break;

	case _HOOK_FREE:
		if (pvData)
			currentAllocated -= (unsigned __int64)_msize_dbg(pvData, nBlockUse);
		break;
	}

	return true;
}

void MemoryTrackInit()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CrtSetAllocHook(AllocHook);
}
#endif

#endif