#include "window.h"
#include "debug.h"

#include <SDL_video.h>
#include <SDL_syswm.h>


struct NativeWindowData
{
	SDL_Window *window;
	SDL_GLContext context;
};


Window::NativeID Window::NativeCreateWindow(
	const char *title,
	int x, int y, int width, int height,
	bool maximize, bool fullscreen,
	int colorBits, int depthBits, int multisample,
	int glMajorVersion, int glMinorVersion)
{
#if defined(PLATFORM_IPHONE)
	fullscreen = true;
#endif
	
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;// | SDL_WINDOW_INPUT_GRABBED; // locks the cursor to the client rect, but interferes with clicking on the menu bar / resizing
	if (fullscreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS;
	}
	else
	{
		flags |= SDL_WINDOW_RESIZABLE;
		if (maximize)
			flags |= SDL_WINDOW_MAXIMIZED;
	}

	if (colorBits >= 24)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
	}
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	
	if (multisample > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
	}
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinorVersion);

// NV GL 4.3 beta driver doesn't seem to be happy with debug contexts... was there a spec change?
//#if defined(_DEBUG)
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_DEBUG, 1);
//#endif
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_CORE, 0);
	
	SDL_Window *window = SDL_CreateWindow(title, x, y, width, height, flags);
	if (!window)
		return 0;

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
	{
		SDL_DestroyWindow(window);
		return 0;
	}

	NativeWindowData *nw = new NativeWindowData;
	nw->window = window;
	nw->context = context;

	return (NativeID)nw;
}

void Window::NativeDestroyWindow(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_GL_DeleteContext(nw->context);
	nw->context = 0;

	SDL_DestroyWindow(nw->window);
	nw->window = 0;

	delete nw;
}

Window::PlatformHandle Window::NativeGetPlatformHandle(NativeID nativeID)
{
#ifdef OS_WINDOWS
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(nw->window, &info))
	{
		return (PlatformHandle)info.info.win.window;
	}
#endif

	return 0;
}

Window::PlatformHandle Window::NativeGetSDLHandle(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	return (PlatformHandle)nw->window;
}

void Window::NativeSetTitle(NativeID nativeID, const char *title)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_SetWindowTitle(nw->window, title);
}

void Window::NativeGetScreenCenter(int testPointX, int testPointY, int &centerX, int &centerY)
{
	int closestDisplay = 0;

	int numDisplays = SDL_GetNumVideoDisplays();
	for (int n = 0; n < numDisplays; n++)
	{
		SDL_Rect rect;
		if (0 != SDL_GetDisplayBounds(n, &rect))
			continue;

		if (testPointX >= rect.x && testPointX < rect.x + rect.w
			&& testPointY >= rect.y && testPointY < rect.y + rect.h)
		{
			closestDisplay = n;
		}
	}

	SDL_Rect closestRect;
	SDL_GetDisplayBounds(closestDisplay, &closestRect);
	centerX = closestRect.x + closestRect.w / 2;
	centerY = closestRect.y + closestRect.h / 2;
}

void Window::NativeGetSize(NativeID nativeID, int &width, int &height)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_GetWindowSize(nw->window, &width, &height);
}

void Window::NativeSetSize(NativeID nativeID, int width, int height)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_SetWindowSize(nw->window, width, height);
}

bool Window::NativeIsVisible(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	int width = 0, height = 0;
	SDL_GetWindowSize(nw->window, &width, &height);

	if (width == 0 || height == 0)
		return false;

	Uint32 flags = SDL_GetWindowFlags(nw->window);
	if (flags & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED))
		return false;

	return true;
}

bool Window::NativeHasFocus(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	Uint32 flags = SDL_GetWindowFlags(nw->window);
	if (flags & SDL_WINDOW_INPUT_FOCUS)
		return true;

	return false;
}

void Window::NativeSetSwapInterval(NativeID nativeID, int interval)
{
	SDL_GL_SetSwapInterval(interval);
}

bool Window::NativeMakeCurrent(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	if (SDL_GL_MakeCurrent(nw->window, nw->context) < 0)
		return false;

	return true;
}

void Window::NativeSwapBuffers(NativeID nativeID)
{
	NativeWindowData *nw = (NativeWindowData*)nativeID;

	SDL_GL_SwapWindow(nw->window);
}
