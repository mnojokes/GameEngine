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

#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include "InputParameters.h"

class App;
class Messenger;

class Input
{
public:
	Input(App* app);
	~Input(void);

	int ProcessInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void Reset(void);

	/* Finalize - creates a dummy input update message without any changes,
	used in frames without input events */
	void Finalize(void);

private:
	void UpdateInputState(void);	// update input states to prepare for the next input event

	void CreateInputMessage(void) const;

private:
	Messenger& m_messenger;

	/* Input states - track current and previous input states */
	bool m_currentKeyState[kNumVirtualKeys];
	bool m_previousKeyState[kNumVirtualKeys];

	/* Mouse coordinates - track current and previous mouse position */
	MouseCoordinates m_previousMouseCoordinates;
	MouseCoordinates m_currentMouseCoordinates;

	/* Mouse wheel delta - track mouse wheel movement. Reset to 0 each frame */
	int m_deltaMouseWheel;

	/* Checks if input message was created that frame. Reset every frame */
	bool m_isInputMessageCreated;
};

#endif
