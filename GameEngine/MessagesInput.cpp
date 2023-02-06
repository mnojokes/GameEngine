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

#include "MessagesInput.h"

bool InputMessage::IsKeyTriggered(VIRTUAL_KEYS key) const
{
	unsigned char cKey = static_cast<unsigned char>(key);
	return m_curKeyState[cKey] && !m_prevKeyState[cKey];	// states: current down, previous up
}

bool InputMessage::IsKeyPressed(VIRTUAL_KEYS key) const
{
	unsigned char cKey = static_cast<unsigned char>(key);
	return m_curKeyState[cKey] && m_prevKeyState[cKey];	// states: current down, previous down
}

bool InputMessage::IsKeyReleased(VIRTUAL_KEYS key) const
{
	unsigned char cKey = static_cast<unsigned char>(key);
	return !m_curKeyState[cKey] && m_prevKeyState[cKey];	// states: current up, previous down
}

bool InputMessage::IsKeyUp(VIRTUAL_KEYS key) const
{
	unsigned char cKey = static_cast<unsigned char>(key);
	return !m_curKeyState[cKey] && !m_prevKeyState[cKey];	// states: current up, previous up
}

bool InputMessage::IsScroll(void) const
{
	return m_deltaMouseWheel != 0;
}

MouseCoordinates InputMessage::GetMousePosition(void) const
{
	return m_curMouseCoordinates;
}

MouseCoordinates InputMessage::GetMouseDeltas(void) const
{
	return { m_curMouseCoordinates - m_prevMouseCoordinates };
}

int InputMessage::GetScrollWheelDelta(void) const
{
	return m_deltaMouseWheel;
}
