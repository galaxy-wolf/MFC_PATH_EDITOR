#include "input.h"

#include <string.h>


const char *Input::keyNameStr[keys] =
{
	"escape",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"minus",
	"equals",
	"back",
	"tab",
	"q",
	"w",
	"e",
	"r",
	"t",
	"y",
	"u",
	"i",
	"o",
	"p",
	"lbracket",
	"rbracket",
	"return",
	"lcontrol",
	"a",
	"s",
	"d",
	"f",
	"g",
	"h",
	"j",
	"k",
	"l",
	"semicolon",
	"apostrophe",
	"grave",
	"lshift",
	"backslash",
	"z",
	"x",
	"c",
	"v",
	"b",
	"n",
	"m",
	"comma",
	"period",
	"slash",
	"rshift",
	"multiply",
	"lalt",
	"space",
	"capital",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"numlock",
	"scroll",
	"numpad7",
	"numpad8",
	"numpad9",
	"subtract",
	"numpad4",
	"numpad5",
	"numpad6",
	"add",
	"numpad1",
	"numpad2",
	"numpad3",
	"numpad0",
	"decimal",
	"f11",
	"f12",
	"numpadenter",
	"rcontrol",
	"divide",
	"sysrq",
	"ralt",
	"pause",
	"home",
	"up",
	"pgup",
	"left",
	"right",
	"end",
	"down",
	"pgdn",
	"insert",
	"delete",
	"lwin",
	"rwin",
	"apps",

	"mouse0",
	"mouse1",
	"mouse2",
	"mouse3",
	"mouse4",
	"mouse5",
	"mouse6",
	"mouse7",
	"mousex",
	"mousey",
	"mousez",

	"joy0_0",
	"joy0_1",
	"joy0_2",
	"joy0_3",
	"joy0_4",
	"joy0_5",
	"joy0_6",
	"joy0_7",
	"joy0_8",
	"joy0_9",
	"joy0_10",
	"joy0_11",
	"joy0_12",
	"joy0_13",
	"joy0_14",
	"joy0_15",
	"joy0_16",
	"joy0_17",
	"joy0_18",
	"joy0_19",
	"joy0_20",
	"joy0_21",
	"joy0_22",
	"joy0_23",
	"joy0_24",
	"joy0_25",
	"joy0_26",
	"joy0_27",
	"joy0_28",
	"joy0_29",
	"joy0_30",
	"joy0_31",

	"joy0_pov0n",
	"joy0_pov0e",
	"joy0_pov0s",
	"joy0_pov0w",

	"joy0_pov1n",
	"joy0_pov1e",
	"joy0_pov1s",
	"joy0_pov1w",

	"joy0_pov2n",
	"joy0_pov2e",
	"joy0_pov2s",
	"joy0_pov2w",

	"joy0_pov3n",
	"joy0_pov3e",
	"joy0_pov3s",
	"joy0_pov3w",

	"joy0_axis0",
	"joy0_axis1",
	"joy0_axis2",
	"joy0_axis3",
	"joy0_axis4",
	"joy0_axis5",
	"joy0_axis6",
	"joy0_axis7",

	"joy1_0",
	"joy1_1",
	"joy1_2",
	"joy1_3",
	"joy1_4",
	"joy1_5",
	"joy1_6",
	"joy1_7",
	"joy1_8",
	"joy1_9",
	"joy1_10",
	"joy1_11",
	"joy1_12",
	"joy1_13",
	"joy1_14",
	"joy1_15",
	"joy1_16",
	"joy1_17",
	"joy1_18",
	"joy1_19",
	"joy1_20",
	"joy1_21",
	"joy1_22",
	"joy1_23",
	"joy1_24",
	"joy1_25",
	"joy1_26",
	"joy1_27",
	"joy1_28",
	"joy1_29",
	"joy1_30",
	"joy1_31",

	"joy1_pov0n",
	"joy1_pov0e",
	"joy1_pov0s",
	"joy1_pov0w",

	"joy1_pov1n",
	"joy1_pov1e",
	"joy1_pov1s",
	"joy1_pov1w",

	"joy1_pov2n",
	"joy1_pov2e",
	"joy1_pov2s",
	"joy1_pov2w",

	"joy1_pov3n",
	"joy1_pov3e",
	"joy1_pov3s",
	"joy1_pov3w",
	
	"joy1_axis0",
	"joy1_axis1",
	"joy1_axis2",
	"joy1_axis3",
	"joy1_axis4",
	"joy1_axis5",
	"joy1_axis6",
	"joy1_axis7",

	"joy2_0",
	"joy2_1",
	"joy2_2",
	"joy2_3",
	"joy2_4",
	"joy2_5",
	"joy2_6",
	"joy2_7",
	"joy2_8",
	"joy2_9",
	"joy2_10",
	"joy2_11",
	"joy2_12",
	"joy2_13",
	"joy2_14",
	"joy2_15",
	"joy2_16",
	"joy2_17",
	"joy2_18",
	"joy2_19",
	"joy2_20",
	"joy2_21",
	"joy2_22",
	"joy2_23",
	"joy2_24",
	"joy2_25",
	"joy2_26",
	"joy2_27",
	"joy2_28",
	"joy2_29",
	"joy2_30",
	"joy2_31",

	"joy2_pov0n",
	"joy2_pov0e",
	"joy2_pov0s",
	"joy2_pov0w",

	"joy2_pov1n",
	"joy2_pov1e",
	"joy2_pov1s",
	"joy2_pov1w",

	"joy2_pov2n",
	"joy2_pov2e",
	"joy2_pov2s",
	"joy2_pov2w",

	"joy2_pov3n",
	"joy2_pov3e",
	"joy2_pov3s",
	"joy2_pov3w",

	"joy2_axis0",
	"joy2_axis1",
	"joy2_axis2",
	"joy2_axis3",
	"joy2_axis4",
	"joy2_axis5",
	"joy2_axis6",
	"joy2_axis7",

	"joy3_0",
	"joy3_1",
	"joy3_2",
	"joy3_3",
	"joy3_4",
	"joy3_5",
	"joy3_6",
	"joy3_7",
	"joy3_8",
	"joy3_9",
	"joy3_10",
	"joy3_11",
	"joy3_12",
	"joy3_13",
	"joy3_14",
	"joy3_15",
	"joy3_16",
	"joy3_17",
	"joy3_18",
	"joy3_19",
	"joy3_20",
	"joy3_21",
	"joy3_22",
	"joy3_23",
	"joy3_24",
	"joy3_25",
	"joy3_26",
	"joy3_27",
	"joy3_28",
	"joy3_29",
	"joy3_30",
	"joy3_31",

	"joy3_pov0n",
	"joy3_pov0e",
	"joy3_pov0s",
	"joy3_pov0w",

	"joy3_pov1n",
	"joy3_pov1e",
	"joy3_pov1s",
	"joy3_pov1w",

	"joy3_pov2n",
	"joy3_pov2e",
	"joy3_pov2s",
	"joy3_pov2w",

	"joy3_pov3n",
	"joy3_pov3e",
	"joy3_pov3s",
	"joy3_pov3w",

	"joy3_axis0",
	"joy3_axis1",
	"joy3_axis2",
	"joy3_axis3",
	"joy3_axis4",
	"joy3_axis5",
	"joy3_axis6",
	"joy3_axis7",

	"accel_pitch",
	"accel_roll",
};

int Input::GetKeyIndexByName(const char *name)
{
	if (!name)
		return -1;

	for (int n = 0; n < keys; n++)
	{
		if (strcmp(name, keyNameStr[n]) == 0)
			return n;
	}

	return -1;
}

Input::Input()
{
	window = 0;
	bindings = 0;
}

Input::~Input()
{
	shutdown();
}

bool Input::startup(Window *_window, bool exclusiveMouseAccessHideCursor)
{
	window = _window;

	restrictCursorToWindow = true;
	ignoreInputOutsideWindow = false;
	hideCursor = true;

	return true;
}

void Input::shutdown()
{
	window = 0;
	bindings = 0;
}

void Input::resetBindings()
{
	bindings = 0;
}

void Input::update()
{
	updateDevices();

	for (unsigned int n = 0; n < bindings; n++)
	{
		if (binding[n].repeat == 0)
		{
			if (binding[n].repeatstate == 0)
			{
				binding[n].repeatstate = getInput(n);
			}
		}
		else if (binding[n].repeat == 2)
		{
			if (getInput(n) == 0)
			{
				binding[n].repeat = 0;
				binding[n].repeatstate = 0;
			}
		}
	}
}

float Input::getState(unsigned int action)
{
	float rval = 0;
	for (unsigned int n = 0; n < bindings; n++)
	{
		if (binding[n].action == action)
		{
			if (binding[n].repeat == 0 && binding[n].repeatstate != 0)
			{
				rval += binding[n].repeatstate;
				binding[n].repeat = 2;
				continue;
			}
			else if (binding[n].repeat == 2)
			{
				continue;
			}

			rval += getInput(n);
		}
	}

	return rval;
}

void Input::bindInput(unsigned int action, unsigned int key, float scale, int repeat)
{
	binding[bindings] = Binding(action, key, scale, repeat);
	bindings++;
}