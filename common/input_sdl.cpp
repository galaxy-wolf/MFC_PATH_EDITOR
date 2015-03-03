#include "input_sdl.h"
#include "window.h"
#include "util.h"
#include "debug.h"
#include "platform.h"

#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_joystick.h>
#include <SDL_events.h>

#ifdef PLATFORM_OSX
# include <Carbon/Carbon.h>
#endif


Input_SDL::Input_SDL()
	: Input()
	, _keyState(0)
	, _pendingKeyPresses(0)
{
	for (int n = 0; n < maxJoysticks; n++)
	{
		_joystick[n] = 0;
		memset(_joystickName[n], 0, sizeof(char) * maxJoystickNameLen);
	}
}

Input_SDL::~Input_SDL()
{
	shutdown();
}

int Input_SDL::getJoystickCount() const
{
	return _joystickCount;
}

const char* Input_SDL::getJoystickName(int index) const
{
	if (index < 0 || index >= _joystickCount)
		return 0;

	return _joystickName[index];
}

bool Input_SDL::startup(Window *_window, bool exclusiveMouseAccessHideCursor)
{
	if (!Input::startup(_window, exclusiveMouseAccessHideCursor))
		return false;

	fillNativeKey();

	// startup code
	_exclusiveMouse = exclusiveMouseAccessHideCursor;

    if (_exclusiveMouse
        && SDL_SetRelativeMouseMode(SDL_TRUE) != -1)
    {
        _relativeMouseSupported = true;
    }
    else
    {
        _relativeMouseSupported = false;
    }
    
	_joystickCount = SDL_NumJoysticks();
	if (_joystickCount > maxJoysticks)
		_joystickCount = maxJoysticks;
	for (int n = 0; n < _joystickCount; n++)
	{
		_joystick[n] = SDL_JoystickOpen(n);

		const char *name = SDL_JoystickName(n);
		if (name)
		{
			strncpy(_joystickName[n], name, maxJoystickNameLen - 1);
			_joystickName[n][maxJoystickNameLen - 1] = 0;
		}
	}

	_keyState = new uint8_t [SDL_NUM_SCANCODES];
	_pendingKeyPresses = new uint8_t [SDL_NUM_SCANCODES];

	// weird... OSX will suppress hardware events (mouse and keyboard events)
	// for a small period of time after you generate a synthetic event
	// (such as warping the cursor)
	// this prevents mouse deltas from being updated, so disable it
	// (only really needed if not using SDL's relative mouse mode under OSX)
#ifdef PLATFORM_OSX
	CGSetLocalEventsSuppressionInterval(0);
#endif
	
	SDL_AddEventWatch(EventWatch, this);

	zeroInputs();
	
	return true;
}

void Input_SDL::shutdown()
{
	for (int n = 0; n < _joystickCount; n++)
	{
		SDL_JoystickClose(_joystick[n]);
		_joystick[n] = 0;
	}

	SDL_DelEventWatch(EventWatch, this);

	delete [] _keyState;
	_keyState = 0;

	delete [] _pendingKeyPresses;
	_pendingKeyPresses = 0;

	Input::shutdown();
}

void Input_SDL::zeroInputs()
{
	memset(_keyState, 0, sizeof(uint8_t) * SDL_NUM_SCANCODES);

	_lastMouseGood = false;
	_mouseDX = 0;
	_mouseDY = 0;
	_mouseDZ = 0;
	_mouseButtonState = 0;

	memset(_joystickState, 0, sizeof(JoystickState) * maxJoysticks);

	zeroPendingInputs();
}

void Input_SDL::zeroPendingInputs()
{
	memset(_pendingKeyPresses, 0, sizeof(uint8_t) * SDL_NUM_SCANCODES);

	_pendingMouseDZ = 0;
	_pendingMouseButtonPresses = 0;

	memset(_pendingJoystickState, 0, sizeof(JoystickState) * maxJoysticks);
}

// could collect pending inputs here to avoid missing fast button presses that happen between frames
int Input_SDL::EventWatch(void *userdata, SDL_Event *event)
{
	Input_SDL *input = (Input_SDL*)userdata;

	switch (event->type)
	{
	case SDL_KEYDOWN:
		// catch initial key presses that happen between frames
		if (event->key.repeat == 0)
		{
			input->_pendingKeyPresses[event->key.keysym.scancode]++;
		}
		break;

	case SDL_KEYUP:
		break;

	case SDL_TEXTEDITING:
		break;

	case SDL_TEXTINPUT:
		break;

	case SDL_MOUSEMOTION:
		break;

	case SDL_MOUSEBUTTONDOWN:
		input->_pendingMouseButtonPresses |= SDL_BUTTON(event->button.button);
		break;

	case SDL_MOUSEBUTTONUP:
		break;

	case SDL_MOUSEWHEEL:
		// could grab horizontal wheel movement here, too, though I don't know how well this is supported
		input->_pendingMouseDZ += event->wheel.y;
		break;

	case SDL_INPUTMOTION:
		break;

	case SDL_INPUTBUTTONDOWN:
		break;

	case SDL_INPUTBUTTONUP:
		break;

	case SDL_INPUTWHEEL:
		break;

	case SDL_INPUTPROXIMITYIN:
		break;

	case SDL_INPUTPROXIMITYOUT:
		break;

	case SDL_JOYAXISMOTION:
		break;
	
	case SDL_JOYBALLMOTION:
		break;

	case SDL_JOYHATMOTION:
		break;

	case SDL_JOYBUTTONDOWN:
		{
			int id = event->jbutton.which;
			int button = event->jbutton.button;
			if (id >= 0 && id < input->_joystickCount && input->_joystick[id]
			&& button < JoystickState::maxButtons)
			{
				input->_pendingJoystickState[id].button[button]++;
			}
		}
		break;

	case SDL_JOYBUTTONUP:
		break;

	case SDL_FINGERDOWN:
		break;

	case SDL_FINGERUP:
		break;

	case SDL_FINGERMOTION:
		break;

	case SDL_TOUCHBUTTONDOWN:
		break;

	case SDL_TOUCHBUTTONUP:
		break;
	}

	return 1;
}

void Input_SDL::updateDevices()
{
	bool cursorShow = false;
	bool cursorHide = false;
	bool acquire = true;

	SDL_Window *sdlWindow = (SDL_Window*)window->getSDLHandle();
	// top-left origin
	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);

	// in window client coordinates
	int mouseX = 0, mouseY = 0;
    int mouseDX = 0, mouseDY = 0;
	Uint8 newMouseButtonState = 0;
    if (_relativeMouseSupported)
    {
        newMouseButtonState = SDL_GetRelativeMouseState(&mouseDX, &mouseDY);
    }
    else
    {
        newMouseButtonState = SDL_GetMouseState(&mouseX, &mouseY);
        mouseDX = mouseX - _lastMouseX;
        mouseDY = mouseY - _lastMouseY;
    }

	if (!window->hasFocus())
	{
		zeroInputs();

		cursorShow = true;
		acquire = false;
	}
	else
	{
		if (_exclusiveMouse)
		{
			cursorHide = true;

			// center the cursor
			mouseX = windowWidth / 2;
			mouseY = windowHeight / 2;
			SDL_WarpMouseInWindow(sdlWindow, mouseX, mouseY);
		}
		else
		{
			if (hideCursor)
			{
				cursorHide = true;
			}
			else
			{
				cursorShow = true;
			}

			if (restrictCursorToWindow)
			{
				// confine the cursor to the window
				int newMouseX = util::clamp(mouseX, 0, windowWidth - 1);
				int newMouseY = util::clamp(mouseY, 0, windowHeight - 1);

				if (newMouseX != mouseX || newMouseY != mouseY)
				{
					mouseX = newMouseX;
					mouseY = newMouseY;
					SDL_WarpMouseInWindow(sdlWindow, mouseX, mouseY);
				}
			}

			if (ignoreInputOutsideWindow)
			{
				if (mouseX < 0 || mouseX >= windowWidth
					|| mouseY < 0 || mouseY >= windowHeight)
				{
					zeroInputs();
					acquire = false;
				}
			}
		}
	}

	if (acquire)
	{
		int numKeys = 0;
		const Uint8 *newKeyState = SDL_GetKeyboardState(&numKeys);
		for (int n = 0; n < SDL_NUM_SCANCODES; n++)
		{
			uint8_t state = 0;
			if (n < numKeys)
				state = newKeyState[n];
			if (_pendingKeyPresses[n] > 0 && state == 0)
				state = 1; // an initial keypress that happened between frames but wasn't caught by the current key state

			_keyState[n] = state;
		}

		if (_lastMouseGood)
		{
			_mouseDX = mouseDX;
			_mouseDY = mouseDY;
			_mouseDZ = _pendingMouseDZ;
		}
		else
		{
			_mouseDX = 0;
			_mouseDY = 0;
			_mouseDZ = 0;
		}
		_mouseButtonState = newMouseButtonState | _pendingMouseButtonPresses;
		_lastMouseX = mouseX;
		_lastMouseY = mouseY;
		_lastMouseGood = true;

		for (int n = 0; n < _joystickCount; n++)
		{
			if (!_joystick[n])
				continue;

			for (int i = 0; i < JoystickState::maxButtons; i++)
			{
				_joystickState[n].button[i] = SDL_JoystickGetButton(_joystick[n], i);

				if (_pendingJoystickState[n].button[i] > 0 && _joystickState[n].button[i] == 0)
				{
					_joystickState[n].button[i] = 1;
				}
			}

			for (int i = 0; i < JoystickState::maxHats; i++)
			{
				uint8_t hatState = SDL_JoystickGetHat(_joystick[n], i);
				_joystickState[n].hat[i][0] = hatState & SDL_HAT_UP;
				_joystickState[n].hat[i][1] = hatState & SDL_HAT_RIGHT;
				_joystickState[n].hat[i][2] = hatState & SDL_HAT_DOWN;
				_joystickState[n].hat[i][3] = hatState & SDL_HAT_LEFT;
			}

			for (int i = 0; i < JoystickState::maxAxis; i++)
			{
				_joystickState[n].axis[i] = SDL_JoystickGetAxis(_joystick[n], i);
			}
		}

		zeroPendingInputs();
	}

	if (cursorShow)
		SDL_ShowCursor(1);
	if (cursorHide)
		SDL_ShowCursor(0);
}

float Input_SDL::getInput(unsigned int i)
{
	float rval = 0;
	int inputVal = _nativeKey[binding[i].key];
	int joyIndex = -1;
	if (inputVal >= 1100 && inputVal < 1100 + 100 * maxJoysticks)
	{
		joyIndex = util::clamp(inputVal / 100 - 11, 0, maxJoysticks - 1);
		inputVal -= joyIndex * 100;
		if (joyIndex >= _joystickCount)
		{
			return 0.0f;
		}
	}
	switch (inputVal)
	{
	case 1000:
	case 1001:
	case 1002:
	case 1003:
	case 1004:
	case 1005:
	case 1006:
	case 1007: // mouse buttons
		{
			int button = inputVal - 1000;
			int buttonMask = SDL_BUTTON(button + 1);
			if (_mouseButtonState & buttonMask)
				rval += binding[i].scale;
		}
		break;
	case 1008: // mousex
		rval += _mouseDX * binding[i].scale;
		break;
	case 1009: // mousey
		rval += -_mouseDY * binding[i].scale;
		break;
	case 1010: // mousez - this has to be handled differently, since return values are weird
		if (_mouseDZ > 0)
			rval += 1 * binding[i].scale;
		else if (_mouseDZ < 0)
			rval += -1 * binding[i].scale;
		break;

	case 1100:
	case 1101:
	case 1102:
	case 1103:
	case 1104:
	case 1105:
	case 1106:
	case 1107:
	case 1108:
	case 1109:
	case 1110:
	case 1111:
	case 1112:
	case 1113:
	case 1114:
	case 1115:
	case 1116:
	case 1117:
	case 1118:
	case 1119:
	case 1120:
	case 1121:
	case 1122:
	case 1123:
	case 1124:
	case 1125:
	case 1126:
	case 1127:
	case 1128:
	case 1129:
	case 1130:
	case 1131: // joystick buttons
		rval += (_joystickState[joyIndex].button[inputVal - 1100] > 0) * binding[i].scale;
		break;

	// pov 0
	case 1132:
	case 1133:
	case 1134:
	case 1135:
	// pov 1
	case 1136:
	case 1137:
	case 1138:
	case 1139:
	// pov 2
	case 1140:
	case 1141:
	case 1142:
	case 1143:
	// pov 3
	case 1144:
	case 1145:
	case 1146:
	case 1147:
		{
			int pov = (inputVal - 1132) / 4;
			int dir = (inputVal - 1132) % 4;
			rval += (_joystickState[joyIndex].hat[pov][dir] > 0) * binding[i].scale;
		}
		break;

	case 1148:
	case 1149:
	case 1150:
	case 1151:
	case 1152:
	case 1153:
	case 1154:
	case 1155:
		{
			int axis = inputVal - 1148;
			float t = _joystickState[joyIndex].axis[axis] / 32767.0f;
			if (t < -1.0f)
				t = -1.0f; // -32768 range
			rval += t * binding[i].scale;

			//DebugPrint("joy%d axis%d %f\n", joyIndex, axis, t);
		}
		break;

	default: // keyboard
		if (inputVal < 1 || inputVal > 221)
			break;
		if (_keyState[inputVal])
		{
			rval += binding[i].scale;
		}
		break;
	};

	return rval;
}

void Input_SDL::fillNativeKey()
{
	_nativeKey[key_escape] = SDL_SCANCODE_ESCAPE;
	_nativeKey[key_1] = SDL_SCANCODE_1;
	_nativeKey[key_2] = SDL_SCANCODE_2;
	_nativeKey[key_3] = SDL_SCANCODE_3;
	_nativeKey[key_4] = SDL_SCANCODE_4;
	_nativeKey[key_5] = SDL_SCANCODE_5;
	_nativeKey[key_6] = SDL_SCANCODE_6;
	_nativeKey[key_7] = SDL_SCANCODE_7;
	_nativeKey[key_8] = SDL_SCANCODE_8;
	_nativeKey[key_9] = SDL_SCANCODE_9;
	_nativeKey[key_0] = SDL_SCANCODE_0;
	_nativeKey[key_minus] = SDL_SCANCODE_MINUS;
	_nativeKey[key_equals] = SDL_SCANCODE_EQUALS;
	_nativeKey[key_back] = SDL_SCANCODE_BACKSPACE;
	_nativeKey[key_tab] = SDL_SCANCODE_TAB;
	_nativeKey[key_q] = SDL_SCANCODE_Q;
	_nativeKey[key_w] = SDL_SCANCODE_W;
	_nativeKey[key_e] = SDL_SCANCODE_E;
	_nativeKey[key_r] = SDL_SCANCODE_R;
	_nativeKey[key_t] = SDL_SCANCODE_T;
	_nativeKey[key_y] = SDL_SCANCODE_Y;
	_nativeKey[key_u] = SDL_SCANCODE_U;
	_nativeKey[key_i] = SDL_SCANCODE_I;
	_nativeKey[key_o] = SDL_SCANCODE_O;
	_nativeKey[key_p] = SDL_SCANCODE_P;
	_nativeKey[key_lbracket] = SDL_SCANCODE_LEFTBRACKET;
	_nativeKey[key_rbracket] = SDL_SCANCODE_RIGHTBRACKET;
	_nativeKey[key_return] = SDL_SCANCODE_RETURN;
	_nativeKey[key_lcontrol] = SDL_SCANCODE_LCTRL;
	_nativeKey[key_a] = SDL_SCANCODE_A;
	_nativeKey[key_s] = SDL_SCANCODE_S;
	_nativeKey[key_d] = SDL_SCANCODE_D;
	_nativeKey[key_f] = SDL_SCANCODE_F;
	_nativeKey[key_g] = SDL_SCANCODE_G;
	_nativeKey[key_h] = SDL_SCANCODE_H;
	_nativeKey[key_j] = SDL_SCANCODE_J;
	_nativeKey[key_k] = SDL_SCANCODE_K;
	_nativeKey[key_l] = SDL_SCANCODE_L;
	_nativeKey[key_semicolon] = SDL_SCANCODE_SEMICOLON;
	_nativeKey[key_apostrophe] = SDL_SCANCODE_APOSTROPHE;
	_nativeKey[key_grave] = SDL_SCANCODE_GRAVE;
	_nativeKey[key_lshift] = SDL_SCANCODE_LSHIFT;
	_nativeKey[key_backslash] = SDL_SCANCODE_BACKSLASH;
	_nativeKey[key_z] = SDL_SCANCODE_Z;
	_nativeKey[key_x] = SDL_SCANCODE_X;
	_nativeKey[key_c] = SDL_SCANCODE_C;
	_nativeKey[key_v] = SDL_SCANCODE_V;
	_nativeKey[key_b] = SDL_SCANCODE_B;
	_nativeKey[key_n] = SDL_SCANCODE_N;
	_nativeKey[key_m] = SDL_SCANCODE_M;
	_nativeKey[key_comma] = SDL_SCANCODE_COMMA;
	_nativeKey[key_period] = SDL_SCANCODE_PERIOD;
	_nativeKey[key_slash] = SDL_SCANCODE_SLASH;
	_nativeKey[key_rshift] = SDL_SCANCODE_RSHIFT;
	_nativeKey[key_multiply] = SDL_SCANCODE_KP_MULTIPLY;
	_nativeKey[key_lalt] = SDL_SCANCODE_LALT;
	_nativeKey[key_space] = SDL_SCANCODE_SPACE;
	_nativeKey[key_capital] = SDL_SCANCODE_CAPSLOCK;
	_nativeKey[key_f1] = SDL_SCANCODE_F1;
	_nativeKey[key_f2] = SDL_SCANCODE_F2;
	_nativeKey[key_f3] = SDL_SCANCODE_F3;
	_nativeKey[key_f4] = SDL_SCANCODE_F4;
	_nativeKey[key_f5] = SDL_SCANCODE_F5;
	_nativeKey[key_f6] = SDL_SCANCODE_F6;
	_nativeKey[key_f7] = SDL_SCANCODE_F7;
	_nativeKey[key_f8] = SDL_SCANCODE_F8;
	_nativeKey[key_f9] = SDL_SCANCODE_F9;
	_nativeKey[key_f10] = SDL_SCANCODE_F10;
	_nativeKey[key_numlock] = SDL_SCANCODE_NUMLOCKCLEAR;
	_nativeKey[key_scroll] = SDL_SCANCODE_SCROLLLOCK;
	_nativeKey[key_numpad7] = SDL_SCANCODE_KP_7;
	_nativeKey[key_numpad8] = SDL_SCANCODE_KP_8;
	_nativeKey[key_numpad9] = SDL_SCANCODE_KP_9;
	_nativeKey[key_subtract] = SDL_SCANCODE_KP_MINUS;
	_nativeKey[key_numpad4] = SDL_SCANCODE_KP_4;
	_nativeKey[key_numpad5] = SDL_SCANCODE_KP_5;
	_nativeKey[key_numpad6] = SDL_SCANCODE_KP_6;
	_nativeKey[key_add] = SDL_SCANCODE_KP_PLUS;
	_nativeKey[key_numpad1] = SDL_SCANCODE_KP_1;
	_nativeKey[key_numpad2] = SDL_SCANCODE_KP_2;
	_nativeKey[key_numpad3] = SDL_SCANCODE_KP_3;
	_nativeKey[key_numpad0] = SDL_SCANCODE_KP_0;
	_nativeKey[key_decimal] = SDL_SCANCODE_KP_PERIOD;
	_nativeKey[key_f11] = SDL_SCANCODE_F11;
	_nativeKey[key_f12] = SDL_SCANCODE_F12;
	_nativeKey[key_numpadenter] = SDL_SCANCODE_KP_ENTER;
	_nativeKey[key_rcontrol] = SDL_SCANCODE_RCTRL;
	_nativeKey[key_divide] = SDL_SCANCODE_KP_DIVIDE;
	_nativeKey[key_sysrq] = SDL_SCANCODE_PRINTSCREEN;
	_nativeKey[key_ralt] = SDL_SCANCODE_RALT;
	_nativeKey[key_pause] = SDL_SCANCODE_PAUSE;
	_nativeKey[key_home] = SDL_SCANCODE_HOME;
	_nativeKey[key_up] = SDL_SCANCODE_UP;
	_nativeKey[key_pgup] = SDL_SCANCODE_PAGEUP;
	_nativeKey[key_left] = SDL_SCANCODE_LEFT;
	_nativeKey[key_right] = SDL_SCANCODE_RIGHT;
	_nativeKey[key_end] = SDL_SCANCODE_END;
	_nativeKey[key_down] = SDL_SCANCODE_DOWN;
	_nativeKey[key_pgdn] = SDL_SCANCODE_PAGEDOWN;
	_nativeKey[key_insert] = SDL_SCANCODE_INSERT;
	_nativeKey[key_delete] = SDL_SCANCODE_DELETE;
	_nativeKey[key_lwin] = SDL_SCANCODE_LGUI;
	_nativeKey[key_rwin] = SDL_SCANCODE_RGUI;
	_nativeKey[key_apps] = SDL_SCANCODE_APPLICATION;

	// mouse input
	_nativeKey[key_mouse0] = 1000;
	_nativeKey[key_mouse1] = 1001;
	_nativeKey[key_mouse2] = 1002;
	_nativeKey[key_mouse3] = 1003;
	_nativeKey[key_mouse4] = 1004;
	_nativeKey[key_mouse5] = 1005;
	_nativeKey[key_mouse6] = 1006;
	_nativeKey[key_mouse7] = 1007;
	_nativeKey[key_mousex] = 1008;
	_nativeKey[key_mousey] = 1009;
	_nativeKey[key_mousez] = 1010;

	// joystick0 input
	_nativeKey[key_joy0_0] = 1100;
	_nativeKey[key_joy0_1] = 1101;
	_nativeKey[key_joy0_2] = 1102;
	_nativeKey[key_joy0_3] = 1103;
	_nativeKey[key_joy0_4] = 1104;
	_nativeKey[key_joy0_5] = 1105;
	_nativeKey[key_joy0_6] = 1106;
	_nativeKey[key_joy0_7] = 1107;
	_nativeKey[key_joy0_8] = 1108;
	_nativeKey[key_joy0_9] = 1109;
	_nativeKey[key_joy0_10] = 1110;
	_nativeKey[key_joy0_11] = 1111;
	_nativeKey[key_joy0_12] = 1112;
	_nativeKey[key_joy0_13] = 1113;
	_nativeKey[key_joy0_14] = 1114;
	_nativeKey[key_joy0_15] = 1115;
	_nativeKey[key_joy0_16] = 1116;
	_nativeKey[key_joy0_17] = 1117;
	_nativeKey[key_joy0_18] = 1118;
	_nativeKey[key_joy0_19] = 1119;
	_nativeKey[key_joy0_20] = 1120;
	_nativeKey[key_joy0_21] = 1121;
	_nativeKey[key_joy0_22] = 1122;
	_nativeKey[key_joy0_23] = 1123;
	_nativeKey[key_joy0_24] = 1124;
	_nativeKey[key_joy0_25] = 1125;
	_nativeKey[key_joy0_26] = 1126;
	_nativeKey[key_joy0_27] = 1127;
	_nativeKey[key_joy0_28] = 1128;
	_nativeKey[key_joy0_29] = 1129;
	_nativeKey[key_joy0_30] = 1130;
	_nativeKey[key_joy0_31] = 1131;

	_nativeKey[key_joy0_pov0n] = 1132;
	_nativeKey[key_joy0_pov0e] = 1133;
	_nativeKey[key_joy0_pov0s] = 1134;
	_nativeKey[key_joy0_pov0w] = 1135;

	_nativeKey[key_joy0_pov1n] = 1136;
	_nativeKey[key_joy0_pov1e] = 1137;
	_nativeKey[key_joy0_pov1s] = 1138;
	_nativeKey[key_joy0_pov1w] = 1139;

	_nativeKey[key_joy0_pov2n] = 1140;
	_nativeKey[key_joy0_pov2e] = 1141;
	_nativeKey[key_joy0_pov2s] = 1142;
	_nativeKey[key_joy0_pov2w] = 1143;

	_nativeKey[key_joy0_pov3n] = 1144;
	_nativeKey[key_joy0_pov3e] = 1145;
	_nativeKey[key_joy0_pov3s] = 1146;
	_nativeKey[key_joy0_pov3w] = 1147;

	_nativeKey[key_joy0_axis0] = 1148;
	_nativeKey[key_joy0_axis1] = 1149;
	_nativeKey[key_joy0_axis2] = 1150;
	_nativeKey[key_joy0_axis3] = 1151;
	_nativeKey[key_joy0_axis4] = 1152;
	_nativeKey[key_joy0_axis5] = 1153;
	_nativeKey[key_joy0_axis6] = 1154;
	_nativeKey[key_joy0_axis7] = 1155;

	// joystick1 input
	_nativeKey[key_joy1_0] = 1200;
	_nativeKey[key_joy1_1] = 1201;
	_nativeKey[key_joy1_2] = 1202;
	_nativeKey[key_joy1_3] = 1203;
	_nativeKey[key_joy1_4] = 1204;
	_nativeKey[key_joy1_5] = 1205;
	_nativeKey[key_joy1_6] = 1206;
	_nativeKey[key_joy1_7] = 1207;
	_nativeKey[key_joy1_8] = 1208;
	_nativeKey[key_joy1_9] = 1209;
	_nativeKey[key_joy1_10] = 1210;
	_nativeKey[key_joy1_11] = 1211;
	_nativeKey[key_joy1_12] = 1212;
	_nativeKey[key_joy1_13] = 1213;
	_nativeKey[key_joy1_14] = 1214;
	_nativeKey[key_joy1_15] = 1215;
	_nativeKey[key_joy1_16] = 1216;
	_nativeKey[key_joy1_17] = 1217;
	_nativeKey[key_joy1_18] = 1218;
	_nativeKey[key_joy1_19] = 1219;
	_nativeKey[key_joy1_20] = 1220;
	_nativeKey[key_joy1_21] = 1221;
	_nativeKey[key_joy1_22] = 1222;
	_nativeKey[key_joy1_23] = 1223;
	_nativeKey[key_joy1_24] = 1224;
	_nativeKey[key_joy1_25] = 1225;
	_nativeKey[key_joy1_26] = 1226;
	_nativeKey[key_joy1_27] = 1227;
	_nativeKey[key_joy1_28] = 1228;
	_nativeKey[key_joy1_29] = 1229;
	_nativeKey[key_joy1_30] = 1230;
	_nativeKey[key_joy1_31] = 1231;

	_nativeKey[key_joy1_pov0n] = 1232;
	_nativeKey[key_joy1_pov0e] = 1233;
	_nativeKey[key_joy1_pov0s] = 1234;
	_nativeKey[key_joy1_pov0w] = 1235;

	_nativeKey[key_joy1_pov1n] = 1236;
	_nativeKey[key_joy1_pov1e] = 1237;
	_nativeKey[key_joy1_pov1s] = 1238;
	_nativeKey[key_joy1_pov1w] = 1239;

	_nativeKey[key_joy1_pov2n] = 1240;
	_nativeKey[key_joy1_pov2e] = 1241;
	_nativeKey[key_joy1_pov2s] = 1242;
	_nativeKey[key_joy1_pov2w] = 1243;

	_nativeKey[key_joy1_pov3n] = 1244;
	_nativeKey[key_joy1_pov3e] = 1245;
	_nativeKey[key_joy1_pov3s] = 1246;
	_nativeKey[key_joy1_pov3w] = 1247;

	_nativeKey[key_joy1_axis0] = 1248;
	_nativeKey[key_joy1_axis1] = 1249;
	_nativeKey[key_joy1_axis2] = 1250;
	_nativeKey[key_joy1_axis3] = 1251;
	_nativeKey[key_joy1_axis4] = 1252;
	_nativeKey[key_joy1_axis5] = 1253;
	_nativeKey[key_joy1_axis6] = 1254;
	_nativeKey[key_joy1_axis7] = 1255;

	// joystick2 input
	_nativeKey[key_joy2_0] = 1300;
	_nativeKey[key_joy2_1] = 1301;
	_nativeKey[key_joy2_2] = 1302;
	_nativeKey[key_joy2_3] = 1303;
	_nativeKey[key_joy2_4] = 1304;
	_nativeKey[key_joy2_5] = 1305;
	_nativeKey[key_joy2_6] = 1306;
	_nativeKey[key_joy2_7] = 1307;
	_nativeKey[key_joy2_8] = 1308;
	_nativeKey[key_joy2_9] = 1309;
	_nativeKey[key_joy2_10] = 1310;
	_nativeKey[key_joy2_11] = 1311;
	_nativeKey[key_joy2_12] = 1312;
	_nativeKey[key_joy2_13] = 1313;
	_nativeKey[key_joy2_14] = 1314;
	_nativeKey[key_joy2_15] = 1315;
	_nativeKey[key_joy2_16] = 1316;
	_nativeKey[key_joy2_17] = 1317;
	_nativeKey[key_joy2_18] = 1318;
	_nativeKey[key_joy2_19] = 1319;
	_nativeKey[key_joy2_20] = 1320;
	_nativeKey[key_joy2_21] = 1321;
	_nativeKey[key_joy2_22] = 1322;
	_nativeKey[key_joy2_23] = 1323;
	_nativeKey[key_joy2_24] = 1324;
	_nativeKey[key_joy2_25] = 1325;
	_nativeKey[key_joy2_26] = 1326;
	_nativeKey[key_joy2_27] = 1327;
	_nativeKey[key_joy2_28] = 1328;
	_nativeKey[key_joy2_29] = 1329;
	_nativeKey[key_joy2_30] = 1330;
	_nativeKey[key_joy2_31] = 1331;

	_nativeKey[key_joy2_pov0n] = 1332;
	_nativeKey[key_joy2_pov0e] = 1333;
	_nativeKey[key_joy2_pov0s] = 1334;
	_nativeKey[key_joy2_pov0w] = 1335;

	_nativeKey[key_joy2_pov1n] = 1336;
	_nativeKey[key_joy2_pov1e] = 1337;
	_nativeKey[key_joy2_pov1s] = 1338;
	_nativeKey[key_joy2_pov1w] = 1339;

	_nativeKey[key_joy2_pov2n] = 1340;
	_nativeKey[key_joy2_pov2e] = 1341;
	_nativeKey[key_joy2_pov2s] = 1342;
	_nativeKey[key_joy2_pov2w] = 1343;

	_nativeKey[key_joy2_pov3n] = 1344;
	_nativeKey[key_joy2_pov3e] = 1345;
	_nativeKey[key_joy2_pov3s] = 1346;
	_nativeKey[key_joy2_pov3w] = 1347;

	_nativeKey[key_joy2_axis0] = 1348;
	_nativeKey[key_joy2_axis1] = 1349;
	_nativeKey[key_joy2_axis2] = 1350;
	_nativeKey[key_joy2_axis3] = 1351;
	_nativeKey[key_joy2_axis4] = 1352;
	_nativeKey[key_joy2_axis5] = 1353;
	_nativeKey[key_joy2_axis6] = 1354;
	_nativeKey[key_joy2_axis7] = 1355;

	// joystick3 input
	_nativeKey[key_joy3_0] = 1400;
	_nativeKey[key_joy3_1] = 1401;
	_nativeKey[key_joy3_2] = 1402;
	_nativeKey[key_joy3_3] = 1403;
	_nativeKey[key_joy3_4] = 1404;
	_nativeKey[key_joy3_5] = 1405;
	_nativeKey[key_joy3_6] = 1406;
	_nativeKey[key_joy3_7] = 1407;
	_nativeKey[key_joy3_8] = 1408;
	_nativeKey[key_joy3_9] = 1409;
	_nativeKey[key_joy3_10] = 1410;
	_nativeKey[key_joy3_11] = 1411;
	_nativeKey[key_joy3_12] = 1412;
	_nativeKey[key_joy3_13] = 1413;
	_nativeKey[key_joy3_14] = 1414;
	_nativeKey[key_joy3_15] = 1415;
	_nativeKey[key_joy3_16] = 1416;
	_nativeKey[key_joy3_17] = 1417;
	_nativeKey[key_joy3_18] = 1418;
	_nativeKey[key_joy3_19] = 1419;
	_nativeKey[key_joy3_20] = 1420;
	_nativeKey[key_joy3_21] = 1421;
	_nativeKey[key_joy3_22] = 1422;
	_nativeKey[key_joy3_23] = 1423;
	_nativeKey[key_joy3_24] = 1424;
	_nativeKey[key_joy3_25] = 1425;
	_nativeKey[key_joy3_26] = 1426;
	_nativeKey[key_joy3_27] = 1427;
	_nativeKey[key_joy3_28] = 1428;
	_nativeKey[key_joy3_29] = 1429;
	_nativeKey[key_joy3_30] = 1430;
	_nativeKey[key_joy3_31] = 1431;

	_nativeKey[key_joy3_pov0n] = 1432;
	_nativeKey[key_joy3_pov0e] = 1433;
	_nativeKey[key_joy3_pov0s] = 1434;
	_nativeKey[key_joy3_pov0w] = 1435;

	_nativeKey[key_joy3_pov1n] = 1436;
	_nativeKey[key_joy3_pov1e] = 1437;
	_nativeKey[key_joy3_pov1s] = 1438;
	_nativeKey[key_joy3_pov1w] = 1439;

	_nativeKey[key_joy3_pov2n] = 1440;
	_nativeKey[key_joy3_pov2e] = 1441;
	_nativeKey[key_joy3_pov2s] = 1442;
	_nativeKey[key_joy3_pov2w] = 1443;

	_nativeKey[key_joy3_pov3n] = 1444;
	_nativeKey[key_joy3_pov3e] = 1445;
	_nativeKey[key_joy3_pov3s] = 1446;
	_nativeKey[key_joy3_pov3w] = 1447;

	_nativeKey[key_joy3_axis0] = 1448;
	_nativeKey[key_joy3_axis1] = 1449;
	_nativeKey[key_joy3_axis2] = 1450;
	_nativeKey[key_joy3_axis3] = 1451;
	_nativeKey[key_joy3_axis4] = 1452;
	_nativeKey[key_joy3_axis5] = 1453;
	_nativeKey[key_joy3_axis6] = 1454;
	_nativeKey[key_joy3_axis7] = 1455;
}
