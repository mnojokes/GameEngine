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

#include "Window.h"
#include <cstdio>

Window::Window(void)
	: m_handle(nullptr)
{}

Window::~Window(void)
{}

void Window::Create(const WNDCLASSA& winClass, const char* name, int xPos, int yPos, int width, int height)
{
	m_handle = CreateWindow(
		winClass.lpszClassName,
		name,
		WS_OVERLAPPEDWINDOW,
		xPos, yPos, width, height,
		nullptr,
		nullptr,
		winClass.hInstance,
		nullptr);

	if (m_handle == nullptr)
	{
		DWORD error = GetLastError();
		fprintf(stderr, "Window::%s: Failed to initialize window. Error %lu\n", __func__, error);
		return;
	}
}

void Window::Show(void)
{
	ShowWindow(m_handle, SW_SHOWNORMAL);
}

void Window::Destroy(void)
{
	BOOL success = DestroyWindow(m_handle);
	if (!success)
	{
		DWORD error = GetLastError();
		fprintf(stderr, "Window::%s: Failed to destroy window. Error %lu\n", __func__, error);
		return;
	}

	m_handle = nullptr;
}

const HWND Window::GetHandle(void) const
{
	return m_handle;
}
