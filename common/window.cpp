#include "window.h"
#include "debug.h"
#include "render_includes.h"
#include "util.h"


Window* Window::_windowLookup[Window::maxWindows] = {};


int Window::GetWindowCount()
{
	int count = 0;
	for (int n = 0; n < maxWindows; n++)
	{
		if (_windowLookup[n] != nullptr)
			count++;
	}
	return count;
}

Window* Window::GetWindow(int id)
{
	if (id < 0 || id >= maxWindows)
		return nullptr;

	return _windowLookup[id];
}

void Window::GetScreenCenter(int testPointX, int testPointY, int &centerX, int &centerY)
{
	NativeGetScreenCenter(testPointX, testPointY, centerX, centerY);
}

Window::PlatformHandle Window::getPlatformHandle() const
{
	return NativeGetPlatformHandle(_nativeID);
}

Window::PlatformHandle Window::getSDLHandle() const
{
	return NativeGetSDLHandle(_nativeID);
}

void Window::setTitle(const char *text)
{
}

void Window::getSize(int &width, int &height) const
{
	NativeGetSize(_nativeID, width, height);
}

void Window::setSize(int width, int height)
{
	NativeSetSize(_nativeID, width, height);
}

bool Window::isVisible() const
{
	return NativeIsVisible(_nativeID);
}

bool Window::hasFocus() const
{
	if (!isVisible())
		return false;

	return NativeHasFocus(_nativeID);
}

bool Window::getCursorPos(float &x, float &y) const
{
	x = 0.0f;
	y = 0.0f;

	// NDC, clamp to [-1,1]

	return true;
}

void Window::setSwapInterval(int interval)
{
	NativeSetSwapInterval(_nativeID, interval);
}

void Window::swapBuffers()
{
	NativeSwapBuffers(_nativeID);
}

bool Window::makeCurrent()
{
	return NativeMakeCurrent(_nativeID);

	return true;
}

void Window::bindFramebuffer()
{
#if defined(PLATFORM_IPHONE) || defined(SIMULATE_IPHONE)
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _framebuffer);
#else
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
}

Window::Window()
	: _nativeID(0)
	, _windowID(-1)
{
}

Window::~Window()
{
	shutdown();
}

void Window::shutdown()
{
	if (_nativeID)
	{
		NativeDestroyWindow(_nativeID);
		_nativeID = 0;
	}

	if (_windowID >= 0 && _windowID < maxWindows)
	{
		_windowLookup[_windowID] = nullptr;
		_windowID = -1;
	}
}

bool Window::startup(
	const char *title,
	int x, int y, int width, int height,
	bool maximize, bool fullscreen,
	int colorbits, int depthbits, int multisample,
	int glMajorVersion, int glMinorVersion)
{
	for (_windowID = 0; _windowID < maxWindows; _windowID++)
	{
		if (_windowLookup[_windowID] == nullptr)
			break;
	}
	if (_windowID >= maxWindows)
		return false; // no available slots
	_windowLookup[_windowID] = this;

	_nativeID = NativeCreateWindow(title, x, y, width, height, maximize, fullscreen, colorbits, depthbits, multisample, glMajorVersion, glMinorVersion);
	if (!_nativeID)
	{
		return false;
	}

	NativeMakeCurrent(_nativeID);

#if defined(OS_WINDOWS)
	static bool needToInitGlew = true;
	if (needToInitGlew)
	{
		needToInitGlew = false;
		glewInit();
	}
#endif

#if defined(PLATFORM_IPHONE) || defined(SIMULATE_IPHONE)
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &_framebuffer);
#endif

	return true;
}
