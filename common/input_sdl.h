#pragma once

#include "input.h"
#include "types.h"

union SDL_Event;
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

class Input_SDL : public Input
{
public:

	Input_SDL();
	~Input_SDL();

	bool startup(Window *_window, bool exclusiveMouseAccessHideCursor = true);
	void shutdown();

	int getJoystickCount() const;
	const char* getJoystickName(int index) const;

	void updateDevices();

	float getInput(unsigned int i);

	void fillNativeKey();

protected:

	int _nativeKey[keys];

	bool _exclusiveMouse;
	
	bool _relativeMouseSupported;

	enum {maxJoysticks = 4};
	int _joystickCount;
	SDL_Joystick *_joystick[maxJoysticks];
	enum {maxJoystickNameLen = 128};
	char _joystickName[maxJoysticks][maxJoystickNameLen];

	struct JoystickState
	{
		enum {maxButtons = 32};
		enum {maxHats = 4};
		enum {maxAxis = 8};

		uint8_t button[maxButtons];
		uint8_t hat[maxHats][4]; // N,E,S,W
		int16_t axis[maxAxis];
	};

	// current state
	uint8_t *_keyState;

	bool _lastMouseGood;
	int _lastMouseX;
	int _lastMouseY;
	int _mouseDX;
	int _mouseDY;
	int _mouseDZ;
	uint8_t _mouseButtonState;

	JoystickState _joystickState[maxJoysticks];

	// state pending, collected between frames
	uint8_t *_pendingKeyPresses;

	int _pendingMouseDZ;
	uint8_t _pendingMouseButtonPresses;

	JoystickState _pendingJoystickState[maxJoysticks];

	void zeroInputs();
	void zeroPendingInputs();

	static int EventWatch(void *userdata, SDL_Event *event);
};
