#include "input_dx.h"
#include "window.h"
#include "debug.h"
#include "util.h"

#include <math.h>
#define DIRECTINPUT_VERSION 0x0800
#define INITGUID
#include <dinput.h>

static int mouse_display = 0;

Input_DX::Input_DX() : Input()
{
	dummyWindow = 0;

	di = 0;
	dik = 0;
	dim = 0;
	for (int n = 0; n < maxJoysticks; n++)
	{
		dij[n] = 0;
	}

	dims = 0;
	for (int n = 0; n < maxJoysticks; n++)
	{
		dijs[n] = 0;
		memset(_joystickName[n], 0, sizeof(char) * maxJoystickNameLen);
	}

	_joystickCount = 0;
}

Input_DX::~Input_DX()
{
	shutdown();
}

struct JoystickSearchData
{
	Input_DX *input;
	HWND hWnd;
};

BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	JoystickSearchData *data = (JoystickSearchData*)pvRef;
	Input_DX *input = data->input;

	if (FAILED(input->di->CreateDevice(lpddi->guidInstance, input->dij + input->_joystickCount, NULL)))
	{
		*(input->dij + input->_joystickCount) = 0;
		return DIENUM_STOP;
	}
	(*(input->dij + input->_joystickCount))->SetCooperativeLevel(data->hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	(*(input->dij + input->_joystickCount))->SetDataFormat(&c_dfDIJoystick);
	(*(input->dij + input->_joystickCount))->Acquire();

	strncpy(input->_joystickName[input->_joystickCount], lpddi->tszProductName, Input_DX::maxJoystickNameLen - 1);
	input->_joystickName[input->_joystickCount][Input_DX::maxJoystickNameLen - 1] = 0;

	input->_joystickCount++;
	if (input->_joystickCount < Input_DX::maxJoysticks)
		return DIENUM_CONTINUE;
	return DIENUM_STOP;
}

int Input_DX::getJoystickCount() const
{
	return _joystickCount;
}

const char* Input_DX::getJoystickName(int index) const
{
	if (index < 0 || index >= _joystickCount)
		return 0;

	return _joystickName[index];
}

bool Input_DX::startup(Window *_window, bool exclusiveMouseAccessHideCursor)
{
	if (!Input::startup(_window, exclusiveMouseAccessHideCursor))
		return false;

	fillNativeKey();

	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL)))
		return false;

	if (FAILED(di->CreateDevice(GUID_SysKeyboard, &dik, NULL)))
		return false;
	if (FAILED(dik->SetDataFormat(&c_dfDIKeyboard)))
		return false;
	if (FAILED(dik->SetCooperativeLevel((HWND)window->getPlatformHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		// window is probably not a top-level window owned by this thread
		// use a dummy window
		if (!dummyWindow)
			dummyWindow = CreateWindowEx(0, "Static", "", 0, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), 0);
		if (!dummyWindow)
			return false;
		if (FAILED(dik->SetCooperativeLevel(dummyWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
			return false;
	}
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 10;
	if (FAILED(dik->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
		return false;

	if (FAILED(di->CreateDevice(GUID_SysMouse, &dim, NULL)))
		return false;
	if (FAILED(dim->SetDataFormat(&c_dfDIMouse2)))
		return false;
	DWORD mouseExclusive = DISCL_NONEXCLUSIVE;
	if (exclusiveMouseAccessHideCursor)
		mouseExclusive = DISCL_EXCLUSIVE;
	if (FAILED(dim->SetCooperativeLevel((HWND)window->getPlatformHandle(), DISCL_FOREGROUND | mouseExclusive)))
	{
		// window is probably not a top-level window owned by this thread
		// use a dummy window
		if (!dummyWindow)
			dummyWindow = CreateWindowEx(0, "Static", "", 0, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), 0);
		if (!dummyWindow)
			return false;
		if (FAILED(dim->SetCooperativeLevel(dummyWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
			return false;
	}

	_joystickCount = 0;
	JoystickSearchData j;
	j.input = this;
	j.hWnd = (HWND)window->getPlatformHandle();
	di->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDevicesCallback, &j, DIEDFL_ATTACHEDONLY);

	dims = new DIMOUSESTATE2;
	for (int n = 0; n < maxJoysticks; n++)
	{
		dijs[n] = new DIJOYSTATE;
	}

	return true;
}

void Input_DX::shutdown()
{
	delete dims;
	dims = 0;

	for (int n = 0; n < maxJoysticks; n++)
	{
		delete dijs[n];
		dijs[n] = 0;
	}

	if (di)
    {
        if (dik)
        {
            dik->Unacquire();
            dik->Release();
            dik = NULL;
        }
		if (dim)
		{
			dim->Unacquire();
			dim->Release();
			dim = NULL;
		}
		for (int n = 0; n < maxJoysticks; n++)
		{
			if (dij[n])
			{
				dij[n]->Unacquire();
				dij[n]->Release();
				dij[n] = 0;
			}
		}
        di->Release();
        di = NULL;
    }

	if (dummyWindow)
	{
		DestroyWindow(dummyWindow);
		dummyWindow = 0;
	}

	Input::shutdown();
}

void Input_DX::zeroInputs()
{
	// zero out
	memset(dims, 0, sizeof(DIMOUSESTATE2));
	memset(keybuffer, 0, sizeof(keybuffer));
	for (int n = 0; n < maxJoysticks; n++)
	{
		memset(dijs[n], 0, sizeof(DIJOYSTATE));
		dijs[n]->lRx = 32767;
		dijs[n]->lRy = 32767;
		dijs[n]->lRz = 32767;
		dijs[n]->lX = 32767;
		dijs[n]->lY = 32767;
		dijs[n]->lZ = 32767;
		dijs[n]->rgdwPOV[0] = 1;
		dijs[n]->rgdwPOV[1] = 1;
		dijs[n]->rgdwPOV[2] = 1;
		dijs[n]->rgdwPOV[3] = 1;
		dijs[n]->rglSlider[0] = 32767;
		dijs[n]->rglSlider[1] = 32767;
	}
}

void Input_DX::updateDevices()
{
	HWND ancestor = GetAncestor((HWND)window->getPlatformHandle(), GA_ROOT);
	//HWND focus = GetFocus();
	HWND foreground = GetForegroundWindow();

	bool cursorShow = false;
	bool cursorHide = false;
	bool acquire = true;

	if (!dummyWindow && foreground != (HWND)window->getPlatformHandle() // wouldn't be able to acquire
		|| dummyWindow && foreground != ancestor // whatever we latched onto is not active
		|| !window->isVisible())
	{
		if (dummyWindow)
		{
			// non-exclusive access, acquire and ignore input so it doesn't pile up once we get focus again
			dim->Acquire();
			dim->GetDeviceState(sizeof(DIMOUSESTATE2), dims);
			dik->Acquire();
			dik->GetDeviceState(sizeof(keybuffer), &keybuffer);
		}

		zeroInputs();

		cursorShow = true;
		acquire = false;
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

		// Note that there is a better way to do this, which won't allow the cursor to go outside the window at all:
		// See ClipCursor() in msdn
		if (restrictCursorToWindow)
		{
			// confine the cursor to the window (we may be non-exclusive on a dummy window, so the cursor could move)
			RECT windowRect;
			if (GetWindowRect((HWND)window->getPlatformHandle(), &windowRect))
			{
				POINT cursorPos, newPos;
				if (GetCursorPos(&cursorPos))
				{
					newPos = cursorPos;

					if (cursorPos.x < windowRect.left)
						newPos.x = windowRect.left;
					else if (cursorPos.x >= windowRect.right)
						newPos.x = windowRect.right - 1;

					if (cursorPos.y < windowRect.top)
						newPos.y = windowRect.top;
					else if (cursorPos.y >= windowRect.bottom)
						newPos.y = windowRect.bottom - 1;

					if (newPos.x != cursorPos.x || newPos.y != cursorPos.y)
					{
						SetCursorPos(newPos.x, newPos.y);
					}
				}
			}
		}

		if (ignoreInputOutsideWindow)
		{
			RECT windowRect;
			if (GetWindowRect((HWND)window->getPlatformHandle(), &windowRect))
			{
				POINT cursorPos;
				if (GetCursorPos(&cursorPos))
				{
					bool outOfBounds = false;

					if (cursorPos.x < windowRect.left)
						outOfBounds = true;
					else if (cursorPos.x >= windowRect.right)
						outOfBounds = true;

					if (cursorPos.y < windowRect.top)
						outOfBounds = true;
					else if (cursorPos.y >= windowRect.bottom)
						outOfBounds = true;

					if (outOfBounds)
					{
						zeroInputs();
						acquire = false;
					}
				}
			}
		}
	}

	if (acquire)
	{
		dim->Acquire();
		dim->GetDeviceState(sizeof(DIMOUSESTATE2), dims);
		dik->Acquire();
		dik->GetDeviceState(sizeof(keybuffer), &keybuffer);

		for (int n = 0; n < maxJoysticks; n++)
		{
			if (dij[n])
			{
				dij[n]->Acquire();
				dij[n]->Poll();
				dij[n]->GetDeviceState(sizeof(DIJOYSTATE), dijs[n]);

				//DebugPrint("joy input: P: %d %d %d, R: %d %d %d, S: %d %d, POV: %d %d %d %d\n", dijs[n]->lX, dijs[n]->lY, dijs[n]->lZ, dijs[n]->lRx, dijs[n]->lRy, dijs[n]->lRz, dijs[n]->rglSlider[0], dijs[n]->rglSlider[1], dijs[n]->rgdwPOV[0], dijs[n]->rgdwPOV[1], dijs[n]->rgdwPOV[2], dijs[n]->rgdwPOV[3]);
				//DebugPrint("joy buttons: %d %d %d %d %d %d %d %d %d %d %d %d\n", dijs[n]->rgbButtons[0], dijs[n]->rgbButtons[1], dijs[n]->rgbButtons[2], dijs[n]->rgbButtons[3], dijs[n]->rgbButtons[4], dijs[n]->rgbButtons[5], dijs[n]->rgbButtons[6], dijs[n]->rgbButtons[7], dijs[n]->rgbButtons[8], dijs[n]->rgbButtons[9], dijs[n]->rgbButtons[10], dijs[n]->rgbButtons[11]);
			}
		}
	}

	if (cursorShow)
	{
		do
		{
			mouse_display = ShowCursor(TRUE);
		} while (mouse_display < 0);

		while (mouse_display > 0)
		{
			mouse_display = ShowCursor(FALSE);
		}
	}
	else if (cursorHide)
	{
		do
		{
			mouse_display = ShowCursor(FALSE);
		} while (mouse_display >= 0);

		while (mouse_display < -1)
		{
			mouse_display = ShowCursor(TRUE);
		}
	}
}

float Input_DX::getInput(unsigned int i)
{
	float rval = 0;
	float t;
	int inputVal = _nativeKey[binding[i].key];
	int joyIndex = -1;
	if (inputVal >= 400 && inputVal < 400 + 100 * maxJoysticks)
	{
		joyIndex = util::clamp(inputVal / 100 - 4, 0, maxJoysticks - 1);
		inputVal -= joyIndex * 100;
		if (joyIndex >= _joystickCount)
		{
			return 0.0f;
		}
	}
	switch (inputVal)
	{
	case 300:
	case 301:
	case 302:
	case 303:
	case 304:
	case 305:
	case 306:
	case 307: // mouse buttons
		rval += (dims->rgbButtons[inputVal - 300] > 0) * binding[i].scale;
		break;
	case 308: // mousex
		rval += dims->lX * binding[i].scale;
		break;
	case 309: // mousey
		rval += -dims->lY * binding[i].scale;
		break;
	case 310: // mousez - this has to be handled differently, since return values are weird
		if (dims->lZ > 0)
			rval += 1 * binding[i].scale;
		else if (dims->lZ < 0)
			rval += -1 * binding[i].scale;
		break;

	case 400:
	case 401:
	case 402:
	case 403:
	case 404:
	case 405:
	case 406:
	case 407:
	case 408:
	case 409:
	case 410:
	case 411:
	case 412:
	case 413:
	case 414:
	case 415:
	case 416:
	case 417:
	case 418:
	case 419:
	case 420:
	case 421:
	case 422:
	case 423:
	case 424:
	case 425:
	case 426:
	case 427:
	case 428:
	case 429:
	case 430:
	case 431: // joystick buttons
		rval += (dijs[joyIndex]->rgbButtons[inputVal - 400] > 0) * binding[i].scale;
		break;

	// pov 0
	case 432:
		if (dijs[joyIndex]->rgdwPOV[0] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[0]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[0] <= 4500 || dijs[joyIndex]->rgdwPOV[0] >= 31500)
			rval += binding[i].scale;
		break;
	case 433:
		if (dijs[joyIndex]->rgdwPOV[0] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[0]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[0] >= 4500 && dijs[joyIndex]->rgdwPOV[0] <= 13500)
			rval += binding[i].scale;
		break;
	case 434:
		if (dijs[joyIndex]->rgdwPOV[0] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[0]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[0] >= 13500 && dijs[joyIndex]->rgdwPOV[0] <= 22500)
			rval += binding[i].scale;
		break;
	case 435:
		if (dijs[joyIndex]->rgdwPOV[0] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[0]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[0] >= 22500 && dijs[joyIndex]->rgdwPOV[0] <= 31500)
			rval += binding[i].scale;
		break;

	// pov 1
	case 436:
		if (dijs[joyIndex]->rgdwPOV[1] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[1]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[1] <= 4500 || dijs[joyIndex]->rgdwPOV[1] >= 31500)
			rval += binding[i].scale;
		break;
	case 437:
		if (dijs[joyIndex]->rgdwPOV[1] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[1]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[1] >= 4500 && dijs[joyIndex]->rgdwPOV[1] <= 13500)
			rval += binding[i].scale;
		break;
	case 438:
		if (dijs[joyIndex]->rgdwPOV[1] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[1]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[1] >= 13500 && dijs[joyIndex]->rgdwPOV[1] <= 22500)
			rval += binding[i].scale;
		break;
	case 439:
		if (dijs[joyIndex]->rgdwPOV[1] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[1]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[1] >= 22500 && dijs[joyIndex]->rgdwPOV[1] <= 31500)
			rval += binding[i].scale;
		break;

	// pov 2
	case 440:
		if (dijs[joyIndex]->rgdwPOV[2] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[2]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[2] <= 4500 || dijs[joyIndex]->rgdwPOV[2] >= 31500)
			rval += binding[i].scale;
		break;
	case 441:
		if (dijs[joyIndex]->rgdwPOV[2] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[2]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[2] >= 4500 && dijs[joyIndex]->rgdwPOV[2] <= 13500)
			rval += binding[i].scale;
		break;
	case 442:
		if (dijs[joyIndex]->rgdwPOV[2] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[2]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[2] >= 13500 && dijs[joyIndex]->rgdwPOV[2] <= 22500)
			rval += binding[i].scale;
		break;
	case 443:
		if (dijs[joyIndex]->rgdwPOV[2] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[2]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[2] >= 22500 && dijs[joyIndex]->rgdwPOV[2] <= 31500)
			rval += binding[i].scale;
		break;

	// pov 3
	case 444:
		if (dijs[joyIndex]->rgdwPOV[3] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[3]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[3] <= 4500 || dijs[joyIndex]->rgdwPOV[3] >= 31500)
			rval += binding[i].scale;
		break;
	case 445:
		if (dijs[joyIndex]->rgdwPOV[3] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[3]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[3] >= 4500 && dijs[joyIndex]->rgdwPOV[3] <= 13500)
			rval += binding[i].scale;
		break;
	case 446:
		if (dijs[joyIndex]->rgdwPOV[3] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[3]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[3] >= 13500 && dijs[joyIndex]->rgdwPOV[3] <= 22500)
			rval += binding[i].scale;
		break;
	case 447:
		if (dijs[joyIndex]->rgdwPOV[3] == 1 || LOWORD(dijs[joyIndex]->rgdwPOV[3]) == 65535) // centered
			break;
		if (dijs[joyIndex]->rgdwPOV[3] >= 22500 && dijs[joyIndex]->rgdwPOV[3] <= 31500)
			rval += binding[i].scale;
		break;

	case 448: // axes
		t = dijs[joyIndex]->lX / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 449:
		t = dijs[joyIndex]->lY / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 450:
		t = dijs[joyIndex]->lZ / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 451: // rotation axes
		t = dijs[joyIndex]->lRx / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 452:
		t = dijs[joyIndex]->lRy / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 453:
		t = dijs[joyIndex]->lRz / 32767.0f - 1.0f;
		rval += t * binding[i].scale;
		break;

	case 454: // sliders
		t = dijs[joyIndex]->rglSlider[0] / 65535.0f;
		t = t * 2.0f - 1.0f;
		rval += t * binding[i].scale;
		break;
	case 455:
		t = dijs[joyIndex]->rglSlider[1] / 65535.0f;
		t = t * 2.0f - 1.0f;
		rval += t * binding[i].scale;
		break;

	default: // keyboard
		if (inputVal < 1 || inputVal > 221)
			break;
		if (keybuffer[inputVal] & 0x80)
		{
			rval += binding[i].scale;
		}
		break;
	};

	return rval;
}

void Input_DX::fillNativeKey()
{
	_nativeKey[key_escape] = 1;
	_nativeKey[key_1] = 2;
	_nativeKey[key_2] = 3;
	_nativeKey[key_3] = 4;
	_nativeKey[key_4] = 5;
	_nativeKey[key_5] = 6;
	_nativeKey[key_6] = 7;
	_nativeKey[key_7] = 8;
	_nativeKey[key_8] = 9;
	_nativeKey[key_9] = 10;
	_nativeKey[key_0] = 11;
	_nativeKey[key_minus] = 12;
	_nativeKey[key_equals] = 13;
	_nativeKey[key_back] = 14;
	_nativeKey[key_tab] = 15;
	_nativeKey[key_q] = 16;
	_nativeKey[key_w] = 17;
	_nativeKey[key_e] = 18;
	_nativeKey[key_r] = 19;
	_nativeKey[key_t] = 20;
	_nativeKey[key_y] = 21;
	_nativeKey[key_u] = 22;
	_nativeKey[key_i] = 23;
	_nativeKey[key_o] = 24;
	_nativeKey[key_p] = 25;
	_nativeKey[key_lbracket] = 26;
	_nativeKey[key_rbracket] = 27;
	_nativeKey[key_return] = 28;
	_nativeKey[key_lcontrol] = 29;
	_nativeKey[key_a] = 30;
	_nativeKey[key_s] = 31;
	_nativeKey[key_d] = 32;
	_nativeKey[key_f] = 33;
	_nativeKey[key_g] = 34;
	_nativeKey[key_h] = 35;
	_nativeKey[key_j] = 36;
	_nativeKey[key_k] = 37;
	_nativeKey[key_l] = 38;
	_nativeKey[key_semicolon] = 39;
	_nativeKey[key_apostrophe] = 40;
	_nativeKey[key_grave] = 41;
	_nativeKey[key_lshift] = 42;
	_nativeKey[key_backslash] = 43;
	_nativeKey[key_z] = 44;
	_nativeKey[key_x] = 45;
	_nativeKey[key_c] = 46;
	_nativeKey[key_v] = 47;
	_nativeKey[key_b] = 48;
	_nativeKey[key_n] = 49;
	_nativeKey[key_m] = 50;
	_nativeKey[key_comma] = 51;
	_nativeKey[key_period] = 52;
	_nativeKey[key_slash] = 53;
	_nativeKey[key_rshift] = 54;
	_nativeKey[key_multiply] = 55;
	_nativeKey[key_lalt] = 56;
	_nativeKey[key_space] = 57;
	_nativeKey[key_capital] = 58;
	_nativeKey[key_f1] = 59;
	_nativeKey[key_f2] = 60;
	_nativeKey[key_f3] = 61;
	_nativeKey[key_f4] = 62;
	_nativeKey[key_f5] = 63;
	_nativeKey[key_f6] = 64;
	_nativeKey[key_f7] = 65;
	_nativeKey[key_f8] = 66;
	_nativeKey[key_f9] = 67;
	_nativeKey[key_f10] = 68;
	_nativeKey[key_numlock] = 69;
	_nativeKey[key_scroll] = 70;
	_nativeKey[key_numpad7] = 71;
	_nativeKey[key_numpad8] = 72;
	_nativeKey[key_numpad9] = 73;
	_nativeKey[key_subtract] = 74;
	_nativeKey[key_numpad4] = 75;
	_nativeKey[key_numpad5] = 76;
	_nativeKey[key_numpad6] = 77;
	_nativeKey[key_add] = 78;
	_nativeKey[key_numpad1] = 79;
	_nativeKey[key_numpad2] = 80;
	_nativeKey[key_numpad3] = 81;
	_nativeKey[key_numpad0] = 82;
	_nativeKey[key_decimal] = 83;
	_nativeKey[key_f11] = 87;
	_nativeKey[key_f12] = 88;
	_nativeKey[key_numpadenter] = 156;
	_nativeKey[key_rcontrol] = 157;
	_nativeKey[key_divide] = 181;
	_nativeKey[key_sysrq] = 183;
	_nativeKey[key_ralt] = 184;
	_nativeKey[key_pause] = 197;
	_nativeKey[key_home] = 199;
	_nativeKey[key_up] = 200;
	_nativeKey[key_pgup] = 201;
	_nativeKey[key_left] = 203;
	_nativeKey[key_right] = 205;
	_nativeKey[key_end] = 207;
	_nativeKey[key_down] = 208;
	_nativeKey[key_pgdn] = 209;
	_nativeKey[key_insert] = 210;
	_nativeKey[key_delete] = 211;
	_nativeKey[key_lwin] = 219;
	_nativeKey[key_rwin] = 220;
	_nativeKey[key_apps] = 221;

	// mouse input
	_nativeKey[key_mouse0] = 300;
	_nativeKey[key_mouse1] = 301;
	_nativeKey[key_mouse2] = 302;
	_nativeKey[key_mouse3] = 303;
	_nativeKey[key_mouse4] = 304;
	_nativeKey[key_mouse5] = 305;
	_nativeKey[key_mouse6] = 306;
	_nativeKey[key_mouse7] = 307;
	_nativeKey[key_mousex] = 308;
	_nativeKey[key_mousey] = 309;
	_nativeKey[key_mousez] = 310;

	// joystick0 input
	_nativeKey[key_joy0_0] = 400;
	_nativeKey[key_joy0_1] = 401;
	_nativeKey[key_joy0_2] = 402;
	_nativeKey[key_joy0_3] = 403;
	_nativeKey[key_joy0_4] = 404;
	_nativeKey[key_joy0_5] = 405;
	_nativeKey[key_joy0_6] = 406;
	_nativeKey[key_joy0_7] = 407;
	_nativeKey[key_joy0_8] = 408;
	_nativeKey[key_joy0_9] = 409;
	_nativeKey[key_joy0_10] = 410;
	_nativeKey[key_joy0_11] = 411;
	_nativeKey[key_joy0_12] = 412;
	_nativeKey[key_joy0_13] = 413;
	_nativeKey[key_joy0_14] = 414;
	_nativeKey[key_joy0_15] = 415;
	_nativeKey[key_joy0_16] = 416;
	_nativeKey[key_joy0_17] = 417;
	_nativeKey[key_joy0_18] = 418;
	_nativeKey[key_joy0_19] = 419;
	_nativeKey[key_joy0_20] = 420;
	_nativeKey[key_joy0_21] = 421;
	_nativeKey[key_joy0_22] = 422;
	_nativeKey[key_joy0_23] = 423;
	_nativeKey[key_joy0_24] = 424;
	_nativeKey[key_joy0_25] = 425;
	_nativeKey[key_joy0_26] = 426;
	_nativeKey[key_joy0_27] = 427;
	_nativeKey[key_joy0_28] = 428;
	_nativeKey[key_joy0_29] = 429;
	_nativeKey[key_joy0_30] = 430;
	_nativeKey[key_joy0_31] = 431;

	_nativeKey[key_joy0_pov0n] = 432;
	_nativeKey[key_joy0_pov0e] = 433;
	_nativeKey[key_joy0_pov0s] = 434;
	_nativeKey[key_joy0_pov0w] = 435;

	_nativeKey[key_joy0_pov1n] = 436;
	_nativeKey[key_joy0_pov1e] = 437;
	_nativeKey[key_joy0_pov1s] = 438;
	_nativeKey[key_joy0_pov1w] = 439;

	_nativeKey[key_joy0_pov2n] = 440;
	_nativeKey[key_joy0_pov2e] = 441;
	_nativeKey[key_joy0_pov2s] = 442;
	_nativeKey[key_joy0_pov2w] = 443;

	_nativeKey[key_joy0_pov3n] = 444;
	_nativeKey[key_joy0_pov3e] = 445;
	_nativeKey[key_joy0_pov3s] = 446;
	_nativeKey[key_joy0_pov3w] = 447;

	_nativeKey[key_joy0_axis0] = 448;
	_nativeKey[key_joy0_axis1] = 449;
	_nativeKey[key_joy0_axis2] = 450;
	_nativeKey[key_joy0_axis3] = 451;
	_nativeKey[key_joy0_axis4] = 452;
	_nativeKey[key_joy0_axis5] = 453;
	_nativeKey[key_joy0_axis6] = 454;
	_nativeKey[key_joy0_axis7] = 455;

	// joystick1 input
	_nativeKey[key_joy1_0] = 500;
	_nativeKey[key_joy1_1] = 501;
	_nativeKey[key_joy1_2] = 502;
	_nativeKey[key_joy1_3] = 503;
	_nativeKey[key_joy1_4] = 504;
	_nativeKey[key_joy1_5] = 505;
	_nativeKey[key_joy1_6] = 506;
	_nativeKey[key_joy1_7] = 507;
	_nativeKey[key_joy1_8] = 508;
	_nativeKey[key_joy1_9] = 509;
	_nativeKey[key_joy1_10] = 510;
	_nativeKey[key_joy1_11] = 511;
	_nativeKey[key_joy1_12] = 512;
	_nativeKey[key_joy1_13] = 513;
	_nativeKey[key_joy1_14] = 514;
	_nativeKey[key_joy1_15] = 515;
	_nativeKey[key_joy1_16] = 516;
	_nativeKey[key_joy1_17] = 517;
	_nativeKey[key_joy1_18] = 518;
	_nativeKey[key_joy1_19] = 519;
	_nativeKey[key_joy1_20] = 520;
	_nativeKey[key_joy1_21] = 521;
	_nativeKey[key_joy1_22] = 522;
	_nativeKey[key_joy1_23] = 523;
	_nativeKey[key_joy1_24] = 524;
	_nativeKey[key_joy1_25] = 525;
	_nativeKey[key_joy1_26] = 526;
	_nativeKey[key_joy1_27] = 527;
	_nativeKey[key_joy1_28] = 528;
	_nativeKey[key_joy1_29] = 529;
	_nativeKey[key_joy1_30] = 530;
	_nativeKey[key_joy1_31] = 531;

	_nativeKey[key_joy1_pov0n] = 532;
	_nativeKey[key_joy1_pov0e] = 533;
	_nativeKey[key_joy1_pov0s] = 534;
	_nativeKey[key_joy1_pov0w] = 535;

	_nativeKey[key_joy1_pov1n] = 536;
	_nativeKey[key_joy1_pov1e] = 537;
	_nativeKey[key_joy1_pov1s] = 538;
	_nativeKey[key_joy1_pov1w] = 539;

	_nativeKey[key_joy1_pov2n] = 540;
	_nativeKey[key_joy1_pov2e] = 541;
	_nativeKey[key_joy1_pov2s] = 542;
	_nativeKey[key_joy1_pov2w] = 543;

	_nativeKey[key_joy1_pov3n] = 544;
	_nativeKey[key_joy1_pov3e] = 545;
	_nativeKey[key_joy1_pov3s] = 546;
	_nativeKey[key_joy1_pov3w] = 547;

	_nativeKey[key_joy1_axis0] = 548;
	_nativeKey[key_joy1_axis1] = 549;
	_nativeKey[key_joy1_axis2] = 550;
	_nativeKey[key_joy1_axis3] = 551;
	_nativeKey[key_joy1_axis4] = 552;
	_nativeKey[key_joy1_axis5] = 553;
	_nativeKey[key_joy1_axis6] = 554;
	_nativeKey[key_joy1_axis7] = 555;

	// joystick2 input
	_nativeKey[key_joy2_0] = 600;
	_nativeKey[key_joy2_1] = 601;
	_nativeKey[key_joy2_2] = 602;
	_nativeKey[key_joy2_3] = 603;
	_nativeKey[key_joy2_4] = 604;
	_nativeKey[key_joy2_5] = 605;
	_nativeKey[key_joy2_6] = 606;
	_nativeKey[key_joy2_7] = 607;
	_nativeKey[key_joy2_8] = 608;
	_nativeKey[key_joy2_9] = 609;
	_nativeKey[key_joy2_10] = 610;
	_nativeKey[key_joy2_11] = 611;
	_nativeKey[key_joy2_12] = 612;
	_nativeKey[key_joy2_13] = 613;
	_nativeKey[key_joy2_14] = 614;
	_nativeKey[key_joy2_15] = 615;
	_nativeKey[key_joy2_16] = 616;
	_nativeKey[key_joy2_17] = 617;
	_nativeKey[key_joy2_18] = 618;
	_nativeKey[key_joy2_19] = 619;
	_nativeKey[key_joy2_20] = 620;
	_nativeKey[key_joy2_21] = 621;
	_nativeKey[key_joy2_22] = 622;
	_nativeKey[key_joy2_23] = 623;
	_nativeKey[key_joy2_24] = 624;
	_nativeKey[key_joy2_25] = 625;
	_nativeKey[key_joy2_26] = 626;
	_nativeKey[key_joy2_27] = 627;
	_nativeKey[key_joy2_28] = 628;
	_nativeKey[key_joy2_29] = 629;
	_nativeKey[key_joy2_30] = 630;
	_nativeKey[key_joy2_31] = 631;

	_nativeKey[key_joy2_pov0n] = 632;
	_nativeKey[key_joy2_pov0e] = 633;
	_nativeKey[key_joy2_pov0s] = 634;
	_nativeKey[key_joy2_pov0w] = 635;

	_nativeKey[key_joy2_pov1n] = 636;
	_nativeKey[key_joy2_pov1e] = 637;
	_nativeKey[key_joy2_pov1s] = 638;
	_nativeKey[key_joy2_pov1w] = 639;

	_nativeKey[key_joy2_pov2n] = 640;
	_nativeKey[key_joy2_pov2e] = 641;
	_nativeKey[key_joy2_pov2s] = 642;
	_nativeKey[key_joy2_pov2w] = 643;

	_nativeKey[key_joy2_pov3n] = 644;
	_nativeKey[key_joy2_pov3e] = 645;
	_nativeKey[key_joy2_pov3s] = 646;
	_nativeKey[key_joy2_pov3w] = 647;

	_nativeKey[key_joy2_axis0] = 648;
	_nativeKey[key_joy2_axis1] = 649;
	_nativeKey[key_joy2_axis2] = 650;
	_nativeKey[key_joy2_axis3] = 651;
	_nativeKey[key_joy2_axis4] = 652;
	_nativeKey[key_joy2_axis5] = 653;
	_nativeKey[key_joy2_axis6] = 654;
	_nativeKey[key_joy2_axis7] = 655;

	// joystick3 input
	_nativeKey[key_joy3_0] = 700;
	_nativeKey[key_joy3_1] = 701;
	_nativeKey[key_joy3_2] = 702;
	_nativeKey[key_joy3_3] = 703;
	_nativeKey[key_joy3_4] = 704;
	_nativeKey[key_joy3_5] = 705;
	_nativeKey[key_joy3_6] = 706;
	_nativeKey[key_joy3_7] = 707;
	_nativeKey[key_joy3_8] = 708;
	_nativeKey[key_joy3_9] = 709;
	_nativeKey[key_joy3_10] = 710;
	_nativeKey[key_joy3_11] = 711;
	_nativeKey[key_joy3_12] = 712;
	_nativeKey[key_joy3_13] = 713;
	_nativeKey[key_joy3_14] = 714;
	_nativeKey[key_joy3_15] = 715;
	_nativeKey[key_joy3_16] = 716;
	_nativeKey[key_joy3_17] = 717;
	_nativeKey[key_joy3_18] = 718;
	_nativeKey[key_joy3_19] = 719;
	_nativeKey[key_joy3_20] = 720;
	_nativeKey[key_joy3_21] = 721;
	_nativeKey[key_joy3_22] = 722;
	_nativeKey[key_joy3_23] = 723;
	_nativeKey[key_joy3_24] = 724;
	_nativeKey[key_joy3_25] = 725;
	_nativeKey[key_joy3_26] = 726;
	_nativeKey[key_joy3_27] = 727;
	_nativeKey[key_joy3_28] = 728;
	_nativeKey[key_joy3_29] = 729;
	_nativeKey[key_joy3_30] = 730;
	_nativeKey[key_joy3_31] = 731;

	_nativeKey[key_joy3_pov0n] = 732;
	_nativeKey[key_joy3_pov0e] = 733;
	_nativeKey[key_joy3_pov0s] = 734;
	_nativeKey[key_joy3_pov0w] = 735;

	_nativeKey[key_joy3_pov1n] = 736;
	_nativeKey[key_joy3_pov1e] = 737;
	_nativeKey[key_joy3_pov1s] = 738;
	_nativeKey[key_joy3_pov1w] = 739;

	_nativeKey[key_joy3_pov2n] = 740;
	_nativeKey[key_joy3_pov2e] = 741;
	_nativeKey[key_joy3_pov2s] = 742;
	_nativeKey[key_joy3_pov2w] = 743;

	_nativeKey[key_joy3_pov3n] = 744;
	_nativeKey[key_joy3_pov3e] = 745;
	_nativeKey[key_joy3_pov3s] = 746;
	_nativeKey[key_joy3_pov3w] = 747;

	_nativeKey[key_joy3_axis0] = 748;
	_nativeKey[key_joy3_axis1] = 749;
	_nativeKey[key_joy3_axis2] = 750;
	_nativeKey[key_joy3_axis3] = 751;
	_nativeKey[key_joy3_axis4] = 752;
	_nativeKey[key_joy3_axis5] = 753;
	_nativeKey[key_joy3_axis6] = 754;
	_nativeKey[key_joy3_axis7] = 755;
}
