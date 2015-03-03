#include "platform.h"

#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# ifdef DEBUG_ALLOCATE_CONSOLE
#  include "window.h"
   static bool gConsoleAllocated = false;
   static HANDLE gConsoleOut = 0;
#  define ALLOCATE_CONSOLE
# endif
#endif

#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#include "timer.h"

static bool logAllocated = false;
static FILE *log = 0;

static bool timerReady = false;
static Timer timer;


void _DebugPrint(const char *format, ...)
{
#ifdef ALLOCATE_CONSOLE
	// allocate the console if it doesn't exist already
	// we don't really care about unallocating it, but the function is FreeConsole()
	if (!gConsoleAllocated)
	{
		gConsoleAllocated = true;

		if (AllocConsole())
		{
			gConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			// put the main window on top
			if (Window::GetWindowCount() > 0)
			{
				SetForegroundWindow((HWND)Window::GetWindow(0)->getPlatformHandle());
			}
		}
	}
#endif

	if (format == 0 || format[0] == 0)
		return;

	// assemble the output string
	const int maxText = 8 * 1024;
	char text[maxText];
	va_list valist;

	va_start(valist, format);
	    vsnprintf(text, maxText - 1, format, valist);
	va_end(valist);
	text[maxText - 1] = 0;

#ifdef _WIN32
	// output to debugger
	OutputDebugString(text);
#endif

#ifdef ALLOCATE_CONSOLE
	// output to console (not supplying characters written pointer...)
	DWORD charsWritten = 0;
	if (gConsoleOut)
		WriteConsole(gConsoleOut, text, (DWORD)strlen(text), &charsWritten, 0);
#else
	printf("%s", text);
#endif
}

void _DebugLog(const char *format, ...)
{
	if (!logAllocated)
	{
		logAllocated = true;
		log = fopen("log.txt", "wb");
	}

	if (!timerReady)
	{
		timerReady = true;
		timer.init();
	}

	// assemble the output string
	const int maxText = 8 * 1024;
	char text[maxText];
	va_list valist;

	va_start(valist, format);
	    vsnprintf(text, maxText - 1, format, valist);
	va_end(valist);

	double currentTime = timer.getElapsed();

	_DebugPrint("%.3f: %s", currentTime, text);

	if (log)
	{
		fprintf(log, "%.3f: %s", currentTime, text);
		fflush(log);
	}
}
