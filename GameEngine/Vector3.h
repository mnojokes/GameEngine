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

#ifndef VECTOR3_H
#define VECTOR3_H

class Vector2;
class Vector4;

class Vector3
{
public:
	Vector3(void);
	Vector3(float x, float y);
	Vector3(float x, float y, float z);
	Vector3(const Vector2& rhs);
	Vector3(const Vector3& rhs) = default;
	Vector3(const Vector4& rhs);
	Vector3& operator=(const Vector3& rhs) = default;


	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator-(void) const;
	Vector3 operator*(const Vector3& rhs) const;
	Vector3 operator*(float f) const;
	friend Vector3 operator*(float f, const Vector3& lhs);
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(const Vector3& rhs);
	bool operator==(const Vector3& rhs) const;
	bool operator!=(const Vector3& rhs) const;

	Vector3 Cross(const Vector3& rhs) const;
	float Dot(const Vector3& rhs) const;
	float Magnitude(void) const;
	float MagnitudeSq(void) const;
	Vector3 Normalize(void) const;

public:
	float x;
	float y;
	float z;
};


#endif
