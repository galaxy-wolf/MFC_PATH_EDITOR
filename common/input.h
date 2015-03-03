#ifndef INPUT_H
#define INPUT_H

class Window;

class Input
{
public:

	// add a new input
	void bindInput(unsigned int action, unsigned int key, float scale, int repeat = 1);
	// get an input
	float getState(unsigned int action);

	virtual int getJoystickCount() const = 0;
	virtual const char* getJoystickName(int index) const = 0;

	class Binding
	{
	public:
		unsigned int action;
		unsigned int key;
		float scale;
		int repeat;
		float repeatstate;

		Binding() : action(0), key(0), scale(0), repeat(0), repeatstate(0)
		{
		}

		Binding(unsigned int _action, unsigned int _key, const float _scale, const int _repeat)
		{
			action = _action;
			key = _key;
			scale = _scale;
			repeat = _repeat;
			repeatstate = 0;
		}
	};

	Window*					window;

	enum {maxBindings = 2048};
	Binding binding[maxBindings];
	unsigned int bindings;

	bool restrictCursorToWindow;
	bool ignoreInputOutsideWindow;
	bool hideCursor;

	Input();
	virtual ~Input();

	// exclusiveAccessHideCursor may prevent you from showing the mouse cursor
	// or using the mouse to interact with other windows,
	// but is needed for most apps that need direct access to the mouse without
	// any chance of errant clicks escaping the window and causing it to lose focus
	// (FPS or any game that uses its own cursor)
	virtual bool startup(Window *_window, bool exclusiveMouseAccessHideCursor = true);
	virtual void shutdown();

	void resetBindings();

	void update();
	virtual void updateDevices() = 0;

	virtual float getInput(unsigned int i) = 0;

	// force cursor to stay within window boundaries
	void setRestrictCursorToWindow(bool enable)
	{
		restrictCursorToWindow = enable;
	}

	// ignore input when cursor is outside window boundaries
	void setIgnoreInputOutsideWindow(bool enable)
	{
		ignoreInputOutsideWindow = enable;
	}

	// hide cursor when input can be acquired
	void setHideCursor(bool enable)
	{
		hideCursor = enable;
	}

////// if modifying this, make sure you update the key name string mappings in input.cpp
	enum
	{
		key_escape = 0,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_0,
		key_minus,
		key_equals,
		key_back,
		key_tab,
		key_q,
		key_w,
		key_e,
		key_r,
		key_t,
		key_y,
		key_u,
		key_i,
		key_o,
		key_p,
		key_lbracket,
		key_rbracket,
		key_return,
		key_lcontrol,
		key_a,
		key_s,
		key_d,
		key_f,
		key_g,
		key_h,
		key_j,
		key_k,
		key_l,
		key_semicolon,
		key_apostrophe,
		key_grave,
		key_lshift,
		key_backslash,
		key_z,
		key_x,
		key_c,
		key_v,
		key_b,
		key_n,
		key_m,
		key_comma,
		key_period,
		key_slash,
		key_rshift,
		key_multiply,
		key_lalt,
		key_space,
		key_capital,
		key_f1,
		key_f2,
		key_f3,
		key_f4,
		key_f5,
		key_f6,
		key_f7,
		key_f8,
		key_f9,
		key_f10,
		key_numlock,
		key_scroll,
		key_numpad7,
		key_numpad8,
		key_numpad9,
		key_subtract,
		key_numpad4,
		key_numpad5,
		key_numpad6,
		key_add,
		key_numpad1,
		key_numpad2,
		key_numpad3,
		key_numpad0,
		key_decimal,
		key_f11,
		key_f12,
		key_numpadenter,
		key_rcontrol,
		key_divide,
		key_sysrq,
		key_ralt,
		key_pause,
		key_home,
		key_up,
		key_pgup,
		key_left,
		key_right,
		key_end,
		key_down,
		key_pgdn,
		key_insert,
		key_delete,
		key_lwin,
		key_rwin,
		key_apps,

		// mouse input
		key_mouse0,
		key_mouse1,
		key_mouse2,
		key_mouse3,
		key_mouse4,
		key_mouse5,
		key_mouse6,
		key_mouse7,
		key_mousex,
		key_mousey,
		key_mousez,

		// joystick0 input
		key_joy0_0,
		key_joy0_1,
		key_joy0_2,
		key_joy0_3,
		key_joy0_4,
		key_joy0_5,
		key_joy0_6,
		key_joy0_7,
		key_joy0_8,
		key_joy0_9,
		key_joy0_10,
		key_joy0_11,
		key_joy0_12,
		key_joy0_13,
		key_joy0_14,
		key_joy0_15,
		key_joy0_16,
		key_joy0_17,
		key_joy0_18,
		key_joy0_19,
		key_joy0_20,
		key_joy0_21,
		key_joy0_22,
		key_joy0_23,
		key_joy0_24,
		key_joy0_25,
		key_joy0_26,
		key_joy0_27,
		key_joy0_28,
		key_joy0_29,
		key_joy0_30,
		key_joy0_31,

		key_joy0_pov0n,
		key_joy0_pov0e,
		key_joy0_pov0s,
		key_joy0_pov0w,

		key_joy0_pov1n,
		key_joy0_pov1e,
		key_joy0_pov1s,
		key_joy0_pov1w,

		key_joy0_pov2n,
		key_joy0_pov2e,
		key_joy0_pov2s,
		key_joy0_pov2w,

		key_joy0_pov3n,
		key_joy0_pov3e,
		key_joy0_pov3s,
		key_joy0_pov3w,

		key_joy0_axis0,
		key_joy0_axis1,
		key_joy0_axis2,
		key_joy0_axis3,
		key_joy0_axis4,
		key_joy0_axis5,
		key_joy0_axis6,
		key_joy0_axis7,

		// joystick1 input
		key_joy1_0,
		key_joy1_1,
		key_joy1_2,
		key_joy1_3,
		key_joy1_4,
		key_joy1_5,
		key_joy1_6,
		key_joy1_7,
		key_joy1_8,
		key_joy1_9,
		key_joy1_10,
		key_joy1_11,
		key_joy1_12,
		key_joy1_13,
		key_joy1_14,
		key_joy1_15,
		key_joy1_16,
		key_joy1_17,
		key_joy1_18,
		key_joy1_19,
		key_joy1_20,
		key_joy1_21,
		key_joy1_22,
		key_joy1_23,
		key_joy1_24,
		key_joy1_25,
		key_joy1_26,
		key_joy1_27,
		key_joy1_28,
		key_joy1_29,
		key_joy1_30,
		key_joy1_31,

		key_joy1_pov0n,
		key_joy1_pov0e,
		key_joy1_pov0s,
		key_joy1_pov0w,

		key_joy1_pov1n,
		key_joy1_pov1e,
		key_joy1_pov1s,
		key_joy1_pov1w,

		key_joy1_pov2n,
		key_joy1_pov2e,
		key_joy1_pov2s,
		key_joy1_pov2w,

		key_joy1_pov3n,
		key_joy1_pov3e,
		key_joy1_pov3s,
		key_joy1_pov3w,

		key_joy1_axis0,
		key_joy1_axis1,
		key_joy1_axis2,
		key_joy1_axis3,
		key_joy1_axis4,
		key_joy1_axis5,
		key_joy1_axis6,
		key_joy1_axis7,

		// joystick2 input
		key_joy2_0,
		key_joy2_1,
		key_joy2_2,
		key_joy2_3,
		key_joy2_4,
		key_joy2_5,
		key_joy2_6,
		key_joy2_7,
		key_joy2_8,
		key_joy2_9,
		key_joy2_10,
		key_joy2_11,
		key_joy2_12,
		key_joy2_13,
		key_joy2_14,
		key_joy2_15,
		key_joy2_16,
		key_joy2_17,
		key_joy2_18,
		key_joy2_19,
		key_joy2_20,
		key_joy2_21,
		key_joy2_22,
		key_joy2_23,
		key_joy2_24,
		key_joy2_25,
		key_joy2_26,
		key_joy2_27,
		key_joy2_28,
		key_joy2_29,
		key_joy2_30,
		key_joy2_31,

		key_joy2_pov0n,
		key_joy2_pov0e,
		key_joy2_pov0s,
		key_joy2_pov0w,

		key_joy2_pov1n,
		key_joy2_pov1e,
		key_joy2_pov1s,
		key_joy2_pov1w,

		key_joy2_pov2n,
		key_joy2_pov2e,
		key_joy2_pov2s,
		key_joy2_pov2w,

		key_joy2_pov3n,
		key_joy2_pov3e,
		key_joy2_pov3s,
		key_joy2_pov3w,

		key_joy2_axis0,
		key_joy2_axis1,
		key_joy2_axis2,
		key_joy2_axis3,
		key_joy2_axis4,
		key_joy2_axis5,
		key_joy2_axis6,
		key_joy2_axis7,

		// joystick3 input
		key_joy3_0,
		key_joy3_1,
		key_joy3_2,
		key_joy3_3,
		key_joy3_4,
		key_joy3_5,
		key_joy3_6,
		key_joy3_7,
		key_joy3_8,
		key_joy3_9,
		key_joy3_10,
		key_joy3_11,
		key_joy3_12,
		key_joy3_13,
		key_joy3_14,
		key_joy3_15,
		key_joy3_16,
		key_joy3_17,
		key_joy3_18,
		key_joy3_19,
		key_joy3_20,
		key_joy3_21,
		key_joy3_22,
		key_joy3_23,
		key_joy3_24,
		key_joy3_25,
		key_joy3_26,
		key_joy3_27,
		key_joy3_28,
		key_joy3_29,
		key_joy3_30,
		key_joy3_31,

		key_joy3_pov0n,
		key_joy3_pov0e,
		key_joy3_pov0s,
		key_joy3_pov0w,

		key_joy3_pov1n,
		key_joy3_pov1e,
		key_joy3_pov1s,
		key_joy3_pov1w,

		key_joy3_pov2n,
		key_joy3_pov2e,
		key_joy3_pov2s,
		key_joy3_pov2w,

		key_joy3_pov3n,
		key_joy3_pov3e,
		key_joy3_pov3s,
		key_joy3_pov3w,

		key_joy3_axis0,
		key_joy3_axis1,
		key_joy3_axis2,
		key_joy3_axis3,
		key_joy3_axis4,
		key_joy3_axis5,
		key_joy3_axis6,
		key_joy3_axis7,

		// accelerometer input
		key_accel_pitch,
		key_accel_roll,

		keys,

		keyJoystickStride = key_joy1_0 - key_joy0_0,
	};
////// if modifying this, make sure you update the key name string mappings in input.cpp

	static const char *keyNameStr[keys];
	static int GetKeyIndexByName(const char *name);
};

#endif
