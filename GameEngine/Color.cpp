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

#include "Color.h"
#include "MathLimits.h"

Color::Color(void)
	: m_color(MAX_8BIT << 24)
{}

Color::Color(const Vector3& color)
{
	uint8_t r = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.x);
	uint8_t g = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.y);
	uint8_t b = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.z);
	uint8_t a = MAX_8BIT;

	m_color = (a << 24) | (b << 16) | (g << 8) | r;
}

Color::Color(const Vector4& color)
{
	uint8_t r = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.x);
	uint8_t g = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.y);
	uint8_t b = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.z);
	uint8_t a = static_cast<uint8_t>(MAX_8BIT_FLOAT * color.w);

	m_color = (a << 24) | (b << 16) | (g << 8) | r;
}

Color::Color(float R, float G, float B)
{
	uint8_t r = static_cast<uint8_t>(MAX_8BIT_FLOAT * R);
	uint8_t g = static_cast<uint8_t>(MAX_8BIT_FLOAT * G);
	uint8_t b = static_cast<uint8_t>(MAX_8BIT_FLOAT * B);

	m_color = (MAX_8BIT << 24) | (b << 16) | (g << 8) | r;
}

Color::Color(float R, float G, float B, float A)
{
	uint8_t r = static_cast<uint8_t>(MAX_8BIT_FLOAT * R);
	uint8_t g = static_cast<uint8_t>(MAX_8BIT_FLOAT * G);
	uint8_t b = static_cast<uint8_t>(MAX_8BIT_FLOAT * B);
	uint8_t a = static_cast<uint8_t>(MAX_8BIT_FLOAT * A);

	m_color = (a << 24) | (b << 16) | (g << 8) | r;
}

Vector3 Color::ToVector3(void) const
{
	float r = (m_color & MAX_8BIT) / MAX_8BIT_FLOAT;
	float g = (m_color >> 8 & MAX_8BIT) / MAX_8BIT_FLOAT;
	float b = (m_color >> 16 & MAX_8BIT) / MAX_8BIT_FLOAT;

	return Vector3(r, g, b);
}

Vector4 Color::ToVector4(void) const
{
	float r = (m_color & MAX_8BIT) / MAX_8BIT_FLOAT;
	float g = (m_color >> 8 & MAX_8BIT) / MAX_8BIT_FLOAT;
	float b = (m_color >> 16 & MAX_8BIT) / MAX_8BIT_FLOAT;
	float a = (m_color >> 24 & MAX_8BIT) / MAX_8BIT_FLOAT;

	return Vector4(r, g, b, a);
}
