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

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>

Vector2::Vector2(void)
	: x(0.0f)
	, y(0.0f)
{}

Vector2::Vector2(float x, float y)
	: x(x)
	, y(y)
{}

Vector2::Vector2(const Vector3& rhs)
	: x(rhs.x)
	, y(rhs.y)
{}

Vector2::Vector2(const Vector4& rhs)
	: x(rhs.x)
	, y(rhs.y)
{}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator-(void) const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator*(const Vector2& rhs) const
{
	return Vector2(x * rhs.x, y * rhs.y);
}

Vector2 Vector2::operator*(float f) const
{
	return Vector2(x * f, y * f);
}

Vector2 operator*(float f, const Vector2& lhs)
{
	return Vector2(f * lhs.x, f * lhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs)
{
	*this = *this * rhs;
	return *this;
}

bool Vector2::operator==(const Vector2& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vector2::operator!=(const Vector2& rhs) const
{
	return !(*this == rhs);
}

Vector2 Vector2::Cross(void) const
{
	return Vector2(-y, x);
}

float Vector2::Dot(const Vector2& rhs) const
{
	return (x * rhs.x) + (y * rhs.y);
}

float Vector2::Magnitude(void) const
{
	float magSq = (x * x) + (y * y);
	return sqrt(magSq);
}

float Vector2::MagnitudeSq(void) const
{
	return (x * x) + (y * y);
}

Vector2 Vector2::Normalize(void) const
{
	Vector2 v;

	float mag = Magnitude();
	if (mag != 0.0f)
	{
		v.x = x / mag;
		v.y = y / mag;
	}

	return v;
}
