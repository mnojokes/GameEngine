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

#ifndef VECTOR2_H
#define VECTOR2_H

class Vector3;
class Vector4;

class Vector2
{
public:
	Vector2(void);
	Vector2(float x, float y);
	Vector2(const Vector2& rhs) = default;
	Vector2(const Vector3& rhs);
	Vector2(const Vector4& rhs);
	Vector2& operator=(const Vector2 & rhs) = default;

	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator-(void) const;
	Vector2 operator*(const Vector2& rhs) const;
	Vector2 operator*(float f) const;
	friend Vector2 operator*(float f, const Vector2& lhs);
	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(const Vector2& rhs);
	bool operator==(const Vector2& rhs) const;
	bool operator!=(const Vector2& rhs) const;

	Vector2 Cross(void) const;
	float Dot(const Vector2& rhs) const;
	float Magnitude(void) const;
	float MagnitudeSq(void) const;
	Vector2 Normalize(void) const;

public:
	float x;
	float y;
};

#endif
