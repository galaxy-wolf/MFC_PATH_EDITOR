#ifndef INPUT_WIN32_H
#define INPUT_WIN32_H

#include "input.h"

struct HHOOK__;

class Input_Win32 : public Input
{
	//friend class Engine;

//protected:
public:

	HHOOK__ *mouseHook;

	float	mouseDelta[3];
	short	keybuffer[256];

	int nativeKey[keys];

	Input_Win32();
	~Input_Win32();

	bool startup(Window *_window, bool exclusiveMouseAccessHideCursor = true);
	void shutdown();

	int getJoystickCount() const;
	const char* getJoystickName(int index) const;

	void updateDevices();

	float getInput(unsigned int i);

	void fillNativeKey();

protected:

	void zeroInputs();
};

#endif
