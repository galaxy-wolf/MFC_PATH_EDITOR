#include "platform.h"
#include "critsect.h"

#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <stdlib.h>
#else
# include <pthread.h>
#endif


CriticalSection::CriticalSection()
{
#ifdef OS_WINDOWS
	cs = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	// this could be a problem for pre-windows NT
	//InitializeCriticalSectionAndSpinCount(cs, 0x80001000);
	InitializeCriticalSection((CRITICAL_SECTION*)cs);
#else
	cs = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)cs, 0);
#endif
}

CriticalSection::~CriticalSection()
{
#ifdef OS_WINDOWS
	DeleteCriticalSection((CRITICAL_SECTION*)cs);
	free(cs);
	cs = 0;
#else
	pthread_mutex_destroy((pthread_mutex_t*)cs);
	delete (pthread_mutex_t*)cs;
	cs = 0;
#endif
}

void CriticalSection::enter()
{
#ifdef OS_WINDOWS
	EnterCriticalSection((CRITICAL_SECTION*)cs);
#else
	pthread_mutex_lock((pthread_mutex_t*)cs);
#endif
}

void CriticalSection::leave()
{
#ifdef OS_WINDOWS
	LeaveCriticalSection((CRITICAL_SECTION*)cs);
#else
	pthread_mutex_unlock((pthread_mutex_t*)cs);
#endif
}
