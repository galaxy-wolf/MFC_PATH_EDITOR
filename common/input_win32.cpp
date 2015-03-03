#include "input_win32.h"
#include "window.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static int mouse_display = 0;
static bool mouse_refresh = true;
static bool mouse_capture = false;
static float mouse_x = 0;
static float mouse_y = 0;
static float mouse_dx = 0;
static float mouse_dy = 0;
static float mouse_dz = 0;

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!mouse_capture || nCode < 0)
		return CallNextHookEx(0, nCode, wParam, lParam);

	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT *hookData = (MSLLHOOKSTRUCT*)lParam;

		switch (wParam)
		{
		case WM_MOUSEMOVE:
			if (mouse_refresh)
			{
				mouse_x = (float)hookData->pt.x;
				mouse_y = (float)hookData->pt.y;
				mouse_refresh = false;
			}
			else
			{
				mouse_dx += (float)hookData->pt.x - mouse_x;
				mouse_dy += (float)hookData->pt.y - mouse_y;
			}
			return 1;
		case WM_MOUSEWHEEL:
			mouse_dz += (float)((short)HIWORD(hookData->mouseData) / WHEEL_DELTA);
			return 1;
		}
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}

Input_Win32::Input_Win32() : Input()
{
	mouseHook = 0;
}

Input_Win32::~Input_Win32()
{
	shutdown();
}

int Input_Win32::getJoystickCount() const
{
	return 0;
}

const char* Input_Win32::getJoystickName(int index) const
{
	return 0;
}

bool Input_Win32::startup(Window *_window, bool exclusiveMouseAccessHideCursor)
{
	if (!Input::startup(_window, exclusiveMouseAccessHideCursor))
		return false;

	fillNativeKey();

	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(0), 0);
	if (!mouseHook)
		return false;

	return true;
}

void Input_Win32::shutdown()
{
	if (mouseHook)
	{
		UnhookWindowsHookEx(mouseHook);
		mouseHook = 0;
	}

	Input::shutdown();
}

void Input_Win32::zeroInputs()
{
	// zero inputs
	memset(keybuffer, 0, sizeof(short) * 256);
	mouseDelta[0] = 0;
	mouseDelta[1] = 0;
	mouseDelta[2] = 0;
}

void Input_Win32::updateDevices()
{
	HWND ancestor = GetAncestor((HWND)window->getPlatformHandle(), GA_ROOT);
	//HWND focus = GetFocus();
	HWND foreground = GetForegroundWindow();

	bool cursorShow = false;
	bool cursorHide = false;
	bool acquire = true;

	if (foreground != ancestor // our window, or whatever we latched onto, is not active
		|| !window->isVisible())
	{
		// refresh the last mouse position
		mouse_refresh = true; // recapture mouse origin
		mouse_capture = false;

		zeroInputs();

		cursorShow = true;
		acquire = false;
	}
	else
	{
		mouse_capture = true;

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
			// confine the cursor to the window
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
						mouse_refresh = true; // recapture mouse origin
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
		// poll devices
		for (int n = 0; n < 256; n++)
		{
			keybuffer[n] = GetAsyncKeyState(n);
		}

		mouseDelta[0] = mouse_dx;
		mouseDelta[1] = mouse_dy;
		mouseDelta[2] = mouse_dz;
		POINT point;
		if (GetCursorPos(&point))
		{
			mouse_x = (float)point.x;
			mouse_y = (float)point.y;
		}
		mouse_dx = 0;
		mouse_dy = 0;
		mouse_dz = 0;
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

float Input_Win32::getInput(unsigned int i)
{
	float deadzone = .1f;

	float rval = 0;
//	float t;
	int inputVal = nativeKey[binding[i].key];
	switch (inputVal)
	{
	case 0: // no binding
		break;

	case 308: // mousex
		rval += mouseDelta[0] * binding[i].scale;
		break;
	case 309: // mousey
		rval += -mouseDelta[1] * binding[i].scale;
		break;
	case 310: // mousez - this has to be handled differently, since return values are weird
		if (mouseDelta[2] > 0)
			rval += 1 * binding[i].scale;
		else if (mouseDelta[2] < 0)
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
//		rval += (dijs->rgbButtons[inputVal - 400] > 0) * binding[i].scale;
		break;

	// pov 0
	case 432:
//		if (dijs->rgdwPOV[0] == 1 || LOWORD(dijs->rgdwPOV[0]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[0] <= 4500 || dijs->rgdwPOV[0] > 31500)
//			rval += binding[i].scale;
		break;
	case 433:
//		if (dijs->rgdwPOV[0] == 1 || LOWORD(dijs->rgdwPOV[0]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[0] > 4500 && dijs->rgdwPOV[0] <= 13500)
//			rval += binding[i].scale;
		break;
	case 434:
//		if (dijs->rgdwPOV[0] == 1 || LOWORD(dijs->rgdwPOV[0]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[0] > 13500 && dijs->rgdwPOV[0] <= 22500)
//			rval += binding[i].scale;
		break;
	case 435:
//		if (dijs->rgdwPOV[0] == 1 || LOWORD(dijs->rgdwPOV[0]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[0] > 22500 && dijs->rgdwPOV[0] <= 31500)
//			rval += binding[i].scale;
		break;

	// pov 1
	case 436:
//		if (dijs->rgdwPOV[1] == 1 || LOWORD(dijs->rgdwPOV[1]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[1] <= 4500 || dijs->rgdwPOV[1] > 31500)
//			rval += binding[i].scale;
		break;
	case 437:
//		if (dijs->rgdwPOV[1] == 1 || LOWORD(dijs->rgdwPOV[1]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[1] > 4500 && dijs->rgdwPOV[1] <= 13500)
//			rval += binding[i].scale;
		break;
	case 438:
//		if (dijs->rgdwPOV[1] == 1 || LOWORD(dijs->rgdwPOV[1]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[1] > 13500 && dijs->rgdwPOV[1] <= 22500)
//			rval += binding[i].scale;
		break;
	case 439:
//		if (dijs->rgdwPOV[1] == 1 || LOWORD(dijs->rgdwPOV[1]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[1] > 22500 && dijs->rgdwPOV[1] <= 31500)
//			rval += binding[i].scale;
		break;

	// pov 2
	case 440:
//		if (dijs->rgdwPOV[2] == 1 || LOWORD(dijs->rgdwPOV[2]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[2] <= 4500 || dijs->rgdwPOV[2] > 31500)
//			rval += binding[i].scale;
		break;
	case 441:
//		if (dijs->rgdwPOV[2] == 1 || LOWORD(dijs->rgdwPOV[2]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[2] > 4500 && dijs->rgdwPOV[2] <= 13500)
//			rval += binding[i].scale;
		break;
	case 442:
//		if (dijs->rgdwPOV[2] == 1 || LOWORD(dijs->rgdwPOV[2]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[2] > 13500 && dijs->rgdwPOV[2] <= 22500)
//			rval += binding[i].scale;
		break;
	case 443:
//		if (dijs->rgdwPOV[2] == 1 || LOWORD(dijs->rgdwPOV[2]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[2] > 22500 && dijs->rgdwPOV[2] <= 31500)
//			rval += binding[i].scale;
		break;

	// pov 3
	case 444:
//		if (dijs->rgdwPOV[3] == 1 || LOWORD(dijs->rgdwPOV[3]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[3] <= 4500 || dijs->rgdwPOV[3] > 31500)
//			rval += binding[i].scale;
		break;
	case 445:
//		if (dijs->rgdwPOV[3] == 1 || LOWORD(dijs->rgdwPOV[3]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[3] > 4500 && dijs->rgdwPOV[3] <= 13500)
//			rval += binding[i].scale;
		break;
	case 446:
//		if (dijs->rgdwPOV[3] == 1 || LOWORD(dijs->rgdwPOV[3]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[3] > 13500 && dijs->rgdwPOV[3] <= 22500)
//			rval += binding[i].scale;
		break;
	case 447:
//		if (dijs->rgdwPOV[3] == 1 || LOWORD(dijs->rgdwPOV[3]) == 65535) // centered
//			break;
//		if (dijs->rgdwPOV[3] > 22500 && dijs->rgdwPOV[3] <= 31500)
//			rval += binding[i].scale;
		break;

	case 448:
	case 449: // sliders
		break;

	case 450: // axes
//		t = 1.0f - dijs->lX / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;
	case 451:
//		t = 1.0f - dijs->lY / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;
	case 452:
//		t = 1.0f - dijs->lZ / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;
	case 453: // rotation axes
//		t = 1.0f - dijs->lRx / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;
	case 454:
//		t = 1.0f - dijs->lRy / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;
	case 455:
//		t = 1.0f - dijs->lRz / 32767.0f;
//		if (fabs(t) < deadzone)
//			t = 0;
//		rval += t * binding[i].scale;
		break;

	default: // keyboard
		if (inputVal < 0x00 || inputVal > 0xff)
			break;
		if (keybuffer[inputVal] & 0x8000)
		{
			rval += binding[i].scale;
		}
		break;
	};

	return rval;
}

void Input_Win32::fillNativeKey()
{
	nativeKey[key_escape] = VK_ESCAPE;
	nativeKey[key_1] = '1';
	nativeKey[key_2] = '2';
	nativeKey[key_3] = '3';
	nativeKey[key_4] = '4';
	nativeKey[key_5] = '5';
	nativeKey[key_6] = '6';
	nativeKey[key_7] = '7';
	nativeKey[key_8] = '8';
	nativeKey[key_9] = '9';
	nativeKey[key_0] = '0';
	nativeKey[key_minus] = VK_OEM_MINUS;
	nativeKey[key_equals] = VK_OEM_PLUS;
	nativeKey[key_back] = VK_BACK;
	nativeKey[key_tab] = VK_TAB;
	nativeKey[key_q] = 'Q';
	nativeKey[key_w] = 'W';
	nativeKey[key_e] = 'E';
	nativeKey[key_r] = 'R';
	nativeKey[key_t] = 'T';
	nativeKey[key_y] = 'Y';
	nativeKey[key_u] = 'U';
	nativeKey[key_i] = 'I';
	nativeKey[key_o] = 'O';
	nativeKey[key_p] = 'P';
	nativeKey[key_lbracket] = VK_OEM_4;
	nativeKey[key_rbracket] = VK_OEM_6;
	nativeKey[key_return] = VK_RETURN;
	nativeKey[key_lcontrol] = VK_LCONTROL;
	nativeKey[key_a] = 'A';
	nativeKey[key_s] = 'S';
	nativeKey[key_d] = 'D';
	nativeKey[key_f] = 'F';
	nativeKey[key_g] = 'G';
	nativeKey[key_h] = 'H';
	nativeKey[key_j] = 'J';
	nativeKey[key_k] = 'K';
	nativeKey[key_l] = 'L';
	nativeKey[key_semicolon] = VK_OEM_1;
	nativeKey[key_apostrophe] = VK_OEM_7;
	nativeKey[key_grave] = VK_OEM_3;
	nativeKey[key_lshift] = VK_LSHIFT;
	nativeKey[key_backslash] = VK_OEM_5;
	nativeKey[key_z] = 'Z';
	nativeKey[key_x] = 'X';
	nativeKey[key_c] = 'C';
	nativeKey[key_v] = 'V';
	nativeKey[key_b] = 'B';
	nativeKey[key_n] = 'N';
	nativeKey[key_m] = 'M';
	nativeKey[key_comma] = VK_OEM_COMMA;
	nativeKey[key_period] = VK_OEM_PERIOD;
	nativeKey[key_slash] = VK_OEM_2;
	nativeKey[key_rshift] = VK_RSHIFT;
	nativeKey[key_multiply] = VK_MULTIPLY;
	nativeKey[key_lalt] = VK_LMENU;
	nativeKey[key_space] = VK_SPACE;
	nativeKey[key_capital] = VK_CAPITAL;
	nativeKey[key_f1] = VK_F1;
	nativeKey[key_f2] = VK_F2;
	nativeKey[key_f3] = VK_F3;
	nativeKey[key_f4] = VK_F4;
	nativeKey[key_f5] = VK_F5;
	nativeKey[key_f6] = VK_F6;
	nativeKey[key_f7] = VK_F7;
	nativeKey[key_f8] = VK_F8;
	nativeKey[key_f9] = VK_F9;
	nativeKey[key_f10] = VK_F10;
	nativeKey[key_numlock] = VK_NUMLOCK;
	nativeKey[key_scroll] = VK_SCROLL;
	nativeKey[key_numpad7] = VK_NUMPAD7;
	nativeKey[key_numpad8] = VK_NUMPAD8;
	nativeKey[key_numpad9] = VK_NUMPAD9;
	nativeKey[key_subtract] = VK_SUBTRACT;
	nativeKey[key_numpad4] = VK_NUMPAD4;
	nativeKey[key_numpad5] = VK_NUMPAD5;
	nativeKey[key_numpad6] = VK_NUMPAD6;
	nativeKey[key_add] = VK_ADD;
	nativeKey[key_numpad1] = VK_NUMPAD1;
	nativeKey[key_numpad2] = VK_NUMPAD2;
	nativeKey[key_numpad3] = VK_NUMPAD3;
	nativeKey[key_numpad0] = VK_NUMPAD0;
	nativeKey[key_decimal] = VK_DECIMAL;
	nativeKey[key_f11] = VK_F11;
	nativeKey[key_f12] = VK_F12;
	nativeKey[key_numpadenter] = VK_SEPARATOR;
	nativeKey[key_rcontrol] = VK_RCONTROL;
	nativeKey[key_divide] = VK_DIVIDE;
	nativeKey[key_sysrq] = VK_SNAPSHOT;
	nativeKey[key_ralt] = VK_RMENU;
	nativeKey[key_pause] = VK_PAUSE;
	nativeKey[key_home] = VK_HOME;
	nativeKey[key_up] = VK_UP;
	nativeKey[key_pgup] = VK_PRIOR;
	nativeKey[key_left] = VK_LEFT;
	nativeKey[key_right] = VK_RIGHT;
	nativeKey[key_end] = VK_END;
	nativeKey[key_down] = VK_DOWN;
	nativeKey[key_pgdn] = VK_NEXT;
	nativeKey[key_insert] = VK_INSERT;
	nativeKey[key_delete] = VK_DELETE;
	nativeKey[key_lwin] = VK_LWIN;
	nativeKey[key_rwin] = VK_RWIN;
	nativeKey[key_apps] = VK_APPS;

	// mouse input
	nativeKey[key_mouse0] = VK_LBUTTON;
	nativeKey[key_mouse1] = VK_RBUTTON;
	nativeKey[key_mouse2] = VK_MBUTTON;
	nativeKey[key_mouse3] = VK_XBUTTON1;
	nativeKey[key_mouse4] = VK_XBUTTON2;
	nativeKey[key_mouse5] = 0;
	nativeKey[key_mouse6] = 0;
	nativeKey[key_mouse7] = 0;
	nativeKey[key_mousex] = 308;
	nativeKey[key_mousey] = 309;
	nativeKey[key_mousez] = 310;
}
