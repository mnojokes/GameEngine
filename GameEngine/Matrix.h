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

#ifndef MATRIX_H
#define MATRIX_H

#include <cstdio>
#include <stdint.h>

class Vector2;
class Vector3;
class Vector4;

class Matrix
{
public:
	Matrix(void);
	Matrix(
		float f00, float f01, float f02, float f03,
		float f10, float f11, float f12, float f13,
		float f20, float f21, float f22, float f23,
		float f30, float f31, float f32, float f33);
	Matrix(const Matrix& rhs) = default;
	Matrix& operator=(const Matrix& rhs) = default;

	Matrix operator+(const Matrix& rhs) const;
	Matrix operator-(const Matrix& rhs) const;
	Matrix operator-(void) const;
	Matrix operator*(const Matrix& rhs) const;
	Matrix operator*(float f) const;
	friend Matrix operator*(float f, const Matrix& lhs);
	Vector4 operator*(const Vector2& v) const;
	Vector4 operator*(const Vector4& v) const;
	Matrix operator/(float f) const;
	Matrix& operator+=(const Matrix& rhs);
	Matrix& operator-=(const Matrix& rhs);
	Matrix& operator*=(const Matrix& rhs);
	Matrix& operator*=(float f);
	Matrix& operator/=(float f);

	Matrix Inverse(void) const;
	Matrix Transpose(void) const;

	static Matrix Rotate(const Vector3& angle);
	static Matrix RotateX(float radians);	// pitch
	static Matrix RotateY(float radians);	// yaw
	static Matrix RotateZ(float radians);	// roll
	static Matrix Scale(const Vector3& scale);
	static Matrix Scale(float x, float y, float z);
	static Matrix Translate(const Vector3& translate);
	static Matrix Translate(float x, float y, float z);
	static Matrix Identity(void);

	static Matrix LookAtLH(const Vector3& cameraPos, const Vector3& targetPoint, const Vector3& upVector);
	static Matrix PerspectiveLH(uint32_t width, uint32_t height, float nearPlane, float farPlane);

	void Print(FILE* stream = stderr) const;

public:	
	float grid[4][4];

private:
	float Determinant3x3(
		float f00, float f01, float f02,
		float f10, float f11, float f12,
		float f20, float f21, float f22);
};

#endif
