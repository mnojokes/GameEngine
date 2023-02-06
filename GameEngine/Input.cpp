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

#include "Input.h"
#include "App.h"
#include "Messenger.h"
#include "MessagesInput.h"
#include <windowsx.h>

Input::Input(App* app)
	: m_messenger(app->GetMessenger())
	, m_currentKeyState{}
	, m_previousKeyState{}
	, m_currentMouseCoordinates{}
	, m_previousMouseCoordinates{}
	, m_deltaMouseWheel(0)
	, m_isInputMessageCreated(false)
{}

Input::~Input(void)
{}

int Input::ProcessInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/* Special case: differentiating left/right Shift, Ctrl, Alt keys */
	if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU)
	{
		UINT scancode = (lParam & 0x00ff0000) >> 16;
		bool isExtended = (lParam & 0x01000000) != 0;

		switch (wParam) {
		case VK_SHIFT:
			wParam = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
			break;
		case VK_CONTROL:
			wParam = isExtended ? VK_RCONTROL : VK_LCONTROL;
			break;
		case VK_MENU:
			wParam = isExtended ? VK_RMENU : VK_LMENU;
			break;
		default:
			break;
		}
	}

	switch (uMsg)
	{
		/* Mouse movement */
	case WM_MOUSEMOVE:
		m_currentMouseCoordinates.x = GET_X_LPARAM(lParam);
		m_currentMouseCoordinates.y = GET_Y_LPARAM(lParam);
		break;

		/* Mouse wheel scrolling */
	case WM_MOUSEWHEEL:
		m_deltaMouseWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		break;

		/* Pressed keys */
	case WM_KEYDOWN:	// keyboard
	case WM_SYSKEYDOWN:	// keyboard system key
	case WM_LBUTTONDOWN:	// left mouse
	case WM_RBUTTONDOWN:	// right mouse
	case WM_MBUTTONDOWN:	// middle mouse
	case WM_XBUTTONDOWN:	// additional mouse button
		m_currentKeyState[wParam] = true;
		break;

		/* Released keyboard keys */
	case WM_KEYUP:	// keyboard
	case WM_SYSKEYUP:	// keyboard system key
		m_currentKeyState[wParam] = false;
		break;

		/* Released mouse keys - separate because Windows send wParam as 0 at release */
	case WM_LBUTTONUP:	// left mouse
		wParam = VK_LBUTTON;
		m_currentKeyState[wParam] = false;
		break;
	case WM_RBUTTONUP:	// right mouse
		wParam = VK_RBUTTON;
		m_currentKeyState[wParam] = false;
		break;
	case WM_MBUTTONUP:	// middle mouse
		wParam = VK_MBUTTON;
		m_currentKeyState[wParam] = false;
		break;
	case WM_XBUTTONUP:	// additional mouse button
		/* Not tested - no compatible hardware */
		m_currentKeyState[wParam] = false;
		break;
		/* Special case: left mouse double click */
	case WM_LBUTTONDBLCLK:
		m_currentKeyState[wParam] = true;
		break;
	default:
		break;
	}

	/* Special case: handle simultaneous left and right Shift presses/releases */
	if (m_currentKeyState[VK_LSHIFT] && !(GetKeyState(VK_LSHIFT) & 0x8000))
	{
		m_currentKeyState[VK_LSHIFT] = false;
	}
	if (m_currentKeyState[VK_RSHIFT] && !(GetKeyState(VK_RSHIFT) & 0x8000))
	{
		m_currentKeyState[VK_RSHIFT] = false;
	}

	CreateInputMessage();
	m_isInputMessageCreated = true;
	UpdateInputState();

	return 0;
}

void Input::Reset(void)
{
	memset(m_currentKeyState, 0, sizeof(bool) * kNumVirtualKeys);
	memset(m_previousKeyState, 0, sizeof(bool) * kNumVirtualKeys);
}

void Input::Finalize(void)
{
	if (!m_isInputMessageCreated)
	{
		CreateInputMessage();
	}
	m_isInputMessageCreated = false;
}

void Input::UpdateInputState(void)
{
	memcpy(m_previousKeyState, m_currentKeyState, sizeof(m_previousKeyState));
	m_previousMouseCoordinates = m_currentMouseCoordinates;
	m_deltaMouseWheel = 0;
}

void Input::CreateInputMessage(void) const
{
	InputMessage msg(m_currentKeyState, m_previousKeyState, m_currentMouseCoordinates, m_previousMouseCoordinates, m_deltaMouseWheel);
	m_messenger.Send(msg);
}
