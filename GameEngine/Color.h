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

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include "Vector3.h"
#include "Vector4.h"

// Color: stores RGBA color channel data in a 32 bit variable. Starts from low bits.
struct Color
{
	Color(void);
	Color(const Vector3& color);
	Color(const Vector4& color);
	Color(float R, float G, float B);
	Color(float R, float G, float B, float A);

	Vector3 ToVector3(void) const;
	Vector4 ToVector4(void) const;

	uint32_t m_color;
};

#endif
