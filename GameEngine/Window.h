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

#ifndef WINDOW_H
#define	WINDOW_H

#include <Windows.h>

class Window
{
public:
	Window(void);
	~Window(void);

	void Create(const WNDCLASSA& winClass, const char* name, int xPos, int yPos, int width, int height);
	void Show(void);
	void Destroy(void);

	const HWND GetHandle(void) const;

private:
	HWND m_handle;
};

#endif
