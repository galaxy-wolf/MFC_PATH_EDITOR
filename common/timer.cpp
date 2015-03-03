#include "timer.h"

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <mach/mach.h>
# include <mach/mach_time.h>
#endif

static bool needTimebase = true;
static double timeScale;


Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::init()
{
	if (needTimebase)
	{
		needTimebase = false;
#ifdef _WIN32
		LARGE_INTEGER timebase;
		QueryPerformanceFrequency(&timebase);
		timeScale = 1.0 / (double)timebase.QuadPart;
#else
		mach_timebase_info_data_t timebase;
		mach_timebase_info(&timebase);
		timeScale = (double)timebase.numer / (double)timebase.denom / 1000000000.0;
#endif
	}

#ifdef _WIN32
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	startTime = temp.QuadPart;
#else
	startTime = mach_absolute_time();
#endif
	lastTime = startTime;
	lastFrame = startTime;
}

double Timer::get()
{
#ifdef _WIN32
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	uint64_t currentTime = temp.QuadPart;
#else
	uint64_t currentTime = mach_absolute_time();
#endif

	double rval = (currentTime - lastTime) * timeScale;
	lastTime = currentTime;

	return rval;
}

double Timer::getElapsed()
{
#ifdef _WIN32
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	uint64_t currentTime = temp.QuadPart;
#else
	uint64_t currentTime = mach_absolute_time();
#endif

	return (currentTime - startTime) * timeScale;
}

double Timer::frametime()
{
#ifdef _WIN32
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	uint64_t currentTime = temp.QuadPart;
#else
	uint64_t currentTime = mach_absolute_time();
#endif

	double rval = (currentTime - lastFrame) * timeScale;
	lastFrame = currentTime;

	return rval;
}
