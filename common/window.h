#ifndef WINDOW_H
#define WINDOW_H

#include "platform.h"
#include "types.h"

class Window
{
public:

	typedef uintptr_t PlatformHandle;

	enum {maxWindows = 1};
	
	Window();
	~Window();

	bool startup(
		const char *title,
		int x, int y, int width, int height,
		bool maximize, bool fullscreen,
		int colorbits, int depthbits, int multisample,
		int glMajorVersion = 3, int glMinorVersion = 2);
	void shutdown();

	static int GetWindowCount();
	static Window* GetWindow(int id);

	// finds the center coordinate of the screen closest to testPoint
	static void GetScreenCenter(int testPointX, int testPointY, int &centerX, int &centerY);

	// get the OS handle of the window
	PlatformHandle getPlatformHandle() const;
	PlatformHandle getSDLHandle() const;

	void setTitle(const char *text);

	void getSize(int &width, int &height) const;
	void setSize(int width, int height);

	bool isVisible() const;
	bool hasFocus() const;

	// normalized to [-1, 1]
	bool getCursorPos(float &x, float &y) const;

	void setSwapInterval(int interval);
	void swapBuffers();
	bool makeCurrent();

	void bindFramebuffer();

private:

	typedef uintptr_t NativeID;

	NativeID _nativeID;
    
#if defined(PLATFORM_IPHONE) || defined(SIMULATE_IPHONE)
    int _framebuffer;
#endif

	static Window *_windowLookup[maxWindows];
	int _windowID;

	static NativeID NativeCreateWindow(
		const char *title,
		int x, int y, int width, int height,
		bool maximize, bool fullscreen,
		int colorBits, int depthBits, int multisample,
		int glMajorVersion, int glMinorVersion);
	static void NativeDestroyWindow(NativeID nativeID);
	static PlatformHandle NativeGetPlatformHandle(NativeID nativeID);
	static PlatformHandle NativeGetSDLHandle(NativeID nativeID);
	static void NativeSetTitle(NativeID nativeID, const char *title);
	static void NativeGetScreenCenter(int testPointX, int testPointY, int &centerX, int &centerY);
	static void NativeGetSize(NativeID nativeID, int &width, int &height);
	static void NativeSetSize(NativeID nativeID, int width, int height);
	static bool NativeIsVisible(NativeID nativeID);
	static bool NativeHasFocus(NativeID nativeID);
	static void NativeSetSwapInterval(NativeID nativeID, int interval);
	static bool NativeMakeCurrent(NativeID nativeID);
	static void NativeSwapBuffers(NativeID nativeID);
};

#endif
