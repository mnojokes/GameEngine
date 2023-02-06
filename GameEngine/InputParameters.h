// Copyright (C) 2023  Mantas Naujokas
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef INPUTPARAMETERS_H
#define INPUTPARAMETERS_H

#include <Windows.h>

struct MouseCoordinates
{
	int x;
	int y;

	MouseCoordinates operator-(const MouseCoordinates& rhs) const
	{
		return MouseCoordinates{ x - rhs.x, y - rhs.y };
	}
};

/* Number of keys supported on standard mouse and keyboard */
enum
{
	kNumVirtualKeys = (unsigned char)~0
};

/********************************************************************************
 *	Virtual Keys:	defines virtual key values based on Windows virtual keys.	*
 *					Prefix M_ for mouse, K_ for Keyboard.						*
 ********************************************************************************/
enum class VIRTUAL_KEYS
{
	/* Mouse keys */
	M_LMB = VK_LBUTTON,
	M_RMB = VK_RBUTTON,
	M_MMB = VK_MBUTTON,
	M_X1 = VK_XBUTTON1,
	M_X2 = VK_XBUTTON2,

	/* Keyboard keys*/
	K_BACKSPACE = VK_BACK,
	K_TAB = VK_TAB,
	K_CLEAR = VK_CLEAR,
	K_ENTER = VK_RETURN,
	// K_SHIFT = VK_SHIFT,
	// K_CTRL = VK_CONTROL,
	// K_ALT = VK_MENU,
	K_PAUSE = VK_PAUSE,
	K_CAPSLOCK = VK_CAPITAL,
	K_ESC = VK_ESCAPE,
	K_SPACE = VK_SPACE,
	K_PGUP = VK_PRIOR,
	K_PGDN = VK_NEXT,
	K_END = VK_END,
	K_HOME = VK_HOME,
	K_LEFT = VK_LEFT,
	K_UP = VK_UP,
	K_RIGHT = VK_RIGHT,
	K_DOWN = VK_DOWN,
	K_SELECT = VK_SELECT,
	K_PRINT = VK_PRINT,
	K_EXECUTE = VK_EXECUTE,
	K_PRINTSCR = VK_SNAPSHOT,
	K_INSERT = VK_INSERT,
	K_DEL = VK_DELETE,
	K_HELP = VK_HELP,
	/* Keyboard numbers */
	K_0 = '0',
	K_1,
	K_2,
	K_3,
	K_4,
	K_5,
	K_6,
	K_7,
	K_8,
	K_9,
	/* Keyboard letters */
	K_A = 'A',
	K_B,
	K_C,
	K_D,
	K_E,
	K_F,
	K_G,
	K_H,
	K_I,
	K_J,
	K_K,
	K_L,
	K_M,
	K_N,
	K_O,
	K_P,
	K_Q,
	K_R,
	K_S,
	K_T,
	K_U,
	K_V,
	K_W,
	K_X,
	K_Y,
	K_Z,
	/* Windows keys */
	K_LWIN = VK_LWIN,
	K_RWIN = VK_RWIN,
	K_APPS = VK_APPS,
	K_SLEEP = VK_SLEEP,
	/* Numpad keys */
	K_NUM0 = VK_NUMPAD0,
	K_NUM1,
	K_NUM2,
	K_NUM3,
	K_NUM4,
	K_NUM5,
	K_NUM6,
	K_NUM7,
	K_NUM8,
	K_NUM9,
	K_MULTIPLY = VK_MULTIPLY,
	K_ADD = VK_ADD,
	K_SEPARATOR = VK_SEPARATOR,
	K_SUBTRACT = VK_SUBTRACT,
	K_DECIMAL = VK_DECIMAL,
	K_DIVIDE = VK_DIVIDE,
	/* F keys */
	K_F1 = VK_F1,
	K_F2,
	K_F3,
	K_F4,
	K_F5,
	K_F6,
	K_F7,
	K_F8,
	K_F9,
	K_F10,
	K_F11,
	K_F12,
	K_F13,
	K_F14,
	K_F15,
	K_F16,
	K_F17,
	K_F18,
	K_F19,
	K_F20,
	K_F21,
	K_F22,
	K_F23,
	K_F24,
	K_NUMLOCK = VK_NUMLOCK,
	K_SCROLLLOCK = VK_SCROLL,
	K_LSHIFT = VK_LSHIFT,
	K_RSHIFT = VK_RSHIFT,
	K_LCTRL = VK_LCONTROL,
	K_RCTRL = VK_RCONTROL,
	K_LALT = VK_LMENU,
	K_RALT = VK_RMENU,
	K_SEMICOLON = VK_OEM_1,	// semicolon, colon
	K_PLUS = VK_OEM_PLUS,
	K_COMMA = VK_OEM_COMMA,
	K_MINUS = VK_OEM_MINUS,
	K_PERIOD = VK_OEM_PERIOD,
	K_FWDSLASH = VK_OEM_2,	// fwd slash, question mark
	K_TILDE = VK_OEM_3,	// tick, tilde
	K_LBRACKET = VK_OEM_4,	// left bracket, curly brace
	K_BACKSLASH = VK_OEM_5,
	K_RBRACKET = VK_OEM_6,	// right bracket, curly brace
	K_QUOTE = VK_OEM_7,
};

#endif
