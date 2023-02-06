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

#ifndef VECTOR4_H
#define VECTOR4_H

class Vector2;
class Vector3;
class JSONData;

class Vector4
{
public:
	Vector4(void);
	Vector4(float x, float y, float z);
	Vector4(float x, float y, float z, float w);
	Vector4(const Vector2& rhs);
	Vector4(const Vector3& rhs);
	Vector4(const Vector4& rhs) = default;
	Vector4& operator=(const Vector4& rhs) = default;

	Vector4 operator+(const Vector4& rhs) const;
	Vector4 operator-(const Vector4& rhs) const;
	Vector4 operator-(void) const;
	Vector4 operator*(const Vector4& rhs) const;
	Vector4 operator*(float f) const;
	friend Vector4 operator*(float f, const Vector4& lhs);
	Vector4& operator+=(const Vector4& rhs);
	Vector4& operator-=(const Vector4& rhs);
	Vector4& operator*=(const Vector4& rhs);
	bool operator==(const Vector4& rhs) const;
	bool operator!=(const Vector4& rhs) const;

	Vector4 Cross(const Vector4& rhs) const;
	float Dot(const Vector4& rhs) const;
	float Magnitude(void) const;
	float MagnitudeSq(void) const;
	Vector4 Normalize(void) const;

public:
	float x;
	float y;
	float z;
	float w;
};

#endif
