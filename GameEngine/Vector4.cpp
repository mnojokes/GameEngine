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

#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

Vector4::Vector4(void)
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(1.0f)
{}

Vector4::Vector4(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
	, w(1.0f)
{}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{}

Vector4::Vector4(const Vector2& rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(0.0f)
	, w(1.0f)
{}

Vector4::Vector4(const Vector3& rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(rhs.z)
	, w(1.0f)
{}

Vector4 Vector4::operator+(const Vector4& rhs) const
{
	return Vector4(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector4 Vector4::operator-(const Vector4& rhs) const
{
	return Vector4(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector4 Vector4::operator-(void) const
{
	return Vector4(-x, -y, -z);
}

Vector4 Vector4::operator*(const Vector4& rhs) const
{
	return Vector4(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector4 Vector4::operator*(float f) const
{
	return Vector4(x * f, y * f, z * f);
}

Vector4 operator*(float f, const Vector4& lhs)
{
	return Vector4(lhs.x * f, lhs.y * f, lhs.z * f);
}

Vector4& Vector4::operator+=(const Vector4& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector4& Vector4::operator*=(const Vector4& rhs)
{
	*this = *this * rhs;
	return *this;
}

bool Vector4::operator==(const Vector4& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector4::operator!=(const Vector4& rhs) const
{
	return !(*this == rhs);
}

Vector4 Vector4::Cross(const Vector4& rhs) const
{
	return Vector4(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

float Vector4::Dot(const Vector4& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

float Vector4::Magnitude(void) const
{
	float magSq = (x * x) + (y * y) + (z * z);
	return sqrt(magSq);
}

float Vector4::MagnitudeSq(void) const
{
	return (x * x) + (y * y) + (z * z);
}

Vector4 Vector4::Normalize(void) const
{
	Vector4 v;

	float mag = Magnitude();
	if (mag != 0.0f)
	{
		v.x = x / mag;
		v.y = y / mag;
		v.z = z / mag;
	}

	return v;
}
