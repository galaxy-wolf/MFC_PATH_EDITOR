#ifndef INPUT_DX_H
#define INPUT_DX_H

#include "input.h"

struct IDirectInput8A;
struct IDirectInputDevice8A;
struct _DIMOUSESTATE2;
struct DIJOYSTATE;

struct HWND__;

class Input_DX : public Input
{
public:

	Input_DX();
	~Input_DX();

	bool startup(Window *_window, bool exclusiveMouseAccessHideCursor = true);
	void shutdown();

	int getJoystickCount() const;
	const char* getJoystickName(int index) const;

	void updateDevices();

	float getInput(unsigned int i);

	void fillNativeKey();

//protected:

	enum {maxJoysticks = 4};
	enum {maxJoystickNameLen = 128};

	int _nativeKey[keys];

	int _joystickCount;
	char _joystickName[maxJoysticks][maxJoystickNameLen];

	HWND__*					dummyWindow;

	IDirectInput8A*			di;
	IDirectInputDevice8A*	dik;
	IDirectInputDevice8A*	dim;
	IDirectInputDevice8A*	dij[maxJoysticks];

	_DIMOUSESTATE2*			dims;
	DIJOYSTATE*				dijs[maxJoysticks];
	unsigned char			keybuffer[256];

	void zeroInputs();
};

#endif
