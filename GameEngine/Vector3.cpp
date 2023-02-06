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

#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include <cmath>

Vector3::Vector3(void)
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{}

Vector3::Vector3(float x, float y)
	: x(x)
	, y(y)
	, z(0.0f)
{}

Vector3::Vector3(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{}

Vector3::Vector3(const Vector2& rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(0.0f)
{}

Vector3::Vector3(const Vector4& rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(rhs.z)
{}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator-(void) const
{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const Vector3& rhs) const
{
	return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 Vector3::operator*(float f) const
{
	return Vector3(x * f, y * f, z * f);
}

Vector3 operator*(float f, const Vector3& lhs)
{
	return Vector3(lhs.x * f, lhs.y * f, lhs.z * f);
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs)
{
	*this = *this * rhs;
	return *this;
}

bool Vector3::operator==(const Vector3& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector3::operator!=(const Vector3& rhs) const
{
	return !(*this == rhs);
}

Vector3 Vector3::Cross(const Vector3& rhs) const
{
	return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

float Vector3::Dot(const Vector3& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

float Vector3::Magnitude(void) const
{
	float magSq = (x * x) + (y * y) + (z * z);
	return sqrt(magSq);
}

float Vector3::MagnitudeSq(void) const
{
	return (x * x) + (y * y) + (z * z);
}

Vector3 Vector3::Normalize(void) const
{
	Vector3 v;

	float mag = Magnitude();
	if (mag != 0.0f)
	{
		v.x = x / mag;
		v.y = y / mag;
		v.z = z / mag;
	}

	return v;
}
