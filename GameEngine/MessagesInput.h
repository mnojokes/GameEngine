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

#ifndef MESSAGESINPUT_H
#define MESSAGESINPUT_H

#include "Message.h"
#include "InputParameters.h"

class InputMessage : public Message
{
public:
	MESSAGE_CTOR(InputMessage,
		const bool* curKeyState,
		const bool* prevKeyState,
		const MouseCoordinates& curMouseCoordinates,
		const MouseCoordinates& prevMouseCoordinates,
		int deltaMouseWheel)
		, m_curKeyState{}
		, m_prevKeyState{}
		, m_curMouseCoordinates(curMouseCoordinates)
		, m_prevMouseCoordinates(prevMouseCoordinates)
		, m_deltaMouseWheel(deltaMouseWheel)
	{
		memcpy(m_curKeyState, curKeyState, sizeof(m_curKeyState));
		memcpy(m_prevKeyState, prevKeyState, sizeof(m_prevKeyState));
	}

	/* Input state checkers: accept virtual key values defined in InputParameters.h */
	bool IsKeyTriggered(VIRTUAL_KEYS key) const;
	bool IsKeyPressed(VIRTUAL_KEYS key) const;
	bool IsKeyReleased(VIRTUAL_KEYS key) const;
	bool IsKeyUp(VIRTUAL_KEYS key) const;

	/* Scroll wheel state checker */
	bool IsScroll(void) const;

	/* Get mouse position and deltas */
	MouseCoordinates GetMousePosition(void) const;
	MouseCoordinates GetMouseDeltas(void) const;
	int GetScrollWheelDelta(void) const;

private:
	/* Key states - track current and previous key states */
	bool m_curKeyState[kNumVirtualKeys];
	bool m_prevKeyState[kNumVirtualKeys];

	/* Mouse positions - track current and previous positions */
	MouseCoordinates m_curMouseCoordinates;
	MouseCoordinates m_prevMouseCoordinates;

	/* Mouse wheel delta */
	int m_deltaMouseWheel;
};

#endif
