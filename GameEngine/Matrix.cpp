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

#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathConstants.h"
#include <cmath>

Matrix::Matrix(void)
	: grid{}
{
	grid[0][0] = grid[1][1] = grid[2][2] = grid[3][3] = 1.0f;
}

Matrix::Matrix(
	float f00, float f01, float f02, float f03,
	float f10, float f11, float f12, float f13,
	float f20, float f21, float f22, float f23,
	float f30, float f31, float f32, float f33)
	: grid{ f00, f01, f02, f03,
			f10, f11, f12, f13,
			f20, f21, f22, f23,
			f30, f31, f32, f33	}
{}

Matrix Matrix::operator+(const Matrix& rhs) const
{
	Matrix m(
		grid[0][0] + rhs.grid[0][0], grid[0][1] + rhs.grid[0][1], grid[0][2] + rhs.grid[0][2], grid[0][3] + rhs.grid[0][3],
		grid[1][0] + rhs.grid[1][0], grid[1][1] + rhs.grid[1][1], grid[1][2] + rhs.grid[1][2], grid[1][3] + rhs.grid[1][3],
		grid[2][0] + rhs.grid[2][0], grid[2][1] + rhs.grid[2][1], grid[2][2] + rhs.grid[2][2], grid[2][3] + rhs.grid[2][3],
		grid[3][0] + rhs.grid[3][0], grid[3][1] + rhs.grid[3][1], grid[3][2] + rhs.grid[3][2], grid[3][3] + rhs.grid[3][3]);

	return m;
}

Matrix Matrix::operator-(const Matrix& rhs) const
{
	Matrix m(
		grid[0][0] - rhs.grid[0][0], grid[0][1] - rhs.grid[0][1], grid[0][2] - rhs.grid[0][2], grid[0][3] - rhs.grid[0][3],
		grid[1][0] - rhs.grid[1][0], grid[1][1] - rhs.grid[1][1], grid[1][2] - rhs.grid[1][2], grid[1][3] - rhs.grid[1][3],
		grid[2][0] - rhs.grid[2][0], grid[2][1] - rhs.grid[2][1], grid[2][2] - rhs.grid[2][2], grid[2][3] - rhs.grid[2][3],
		grid[3][0] - rhs.grid[3][0], grid[3][1] - rhs.grid[3][1], grid[3][2] - rhs.grid[3][2], grid[3][3] - rhs.grid[3][3]);

	return m;
}

Matrix Matrix::operator-(void) const
{
	Matrix m(
		-grid[0][0], -grid[0][1], -grid[0][2], -grid[0][3],
		-grid[1][0], -grid[1][1], -grid[1][2], -grid[1][3],
		-grid[2][0], -grid[2][1], -grid[2][2], -grid[2][3],
		-grid[3][0], -grid[3][1], -grid[3][2], -grid[3][3]);

	return m;
}

Matrix Matrix::operator*(const Matrix& rhs) const
{
	Matrix m(
		grid[0][0] * rhs.grid[0][0] + grid[0][1] * rhs.grid[1][0] + grid[0][2] * rhs.grid[2][0] + grid[0][3] * rhs.grid[3][0],
		grid[0][0] * rhs.grid[0][1] + grid[0][1] * rhs.grid[1][1] + grid[0][2] * rhs.grid[2][1] + grid[0][3] * rhs.grid[3][1],
		grid[0][0] * rhs.grid[0][2] + grid[0][1] * rhs.grid[1][2] + grid[0][2] * rhs.grid[2][2] + grid[0][3] * rhs.grid[3][2],
		grid[0][0] * rhs.grid[0][3] + grid[0][1] * rhs.grid[1][3] + grid[0][2] * rhs.grid[2][3] + grid[0][3] * rhs.grid[3][3],
		grid[1][0] * rhs.grid[0][0] + grid[1][1] * rhs.grid[1][0] + grid[1][2] * rhs.grid[2][0] + grid[1][3] * rhs.grid[3][0],
		grid[1][0] * rhs.grid[0][1] + grid[1][1] * rhs.grid[1][1] + grid[1][2] * rhs.grid[2][1] + grid[1][3] * rhs.grid[3][1],
		grid[1][0] * rhs.grid[0][2] + grid[1][1] * rhs.grid[1][2] + grid[1][2] * rhs.grid[2][2] + grid[1][3] * rhs.grid[3][2],
		grid[1][0] * rhs.grid[0][3] + grid[1][1] * rhs.grid[1][3] + grid[1][2] * rhs.grid[2][3] + grid[1][3] * rhs.grid[3][3],
		grid[2][0] * rhs.grid[0][0] + grid[2][1] * rhs.grid[1][0] + grid[2][2] * rhs.grid[2][0] + grid[2][3] * rhs.grid[3][0],
		grid[2][0] * rhs.grid[0][1] + grid[2][1] * rhs.grid[1][1] + grid[2][2] * rhs.grid[2][1] + grid[2][3] * rhs.grid[3][1],
		grid[2][0] * rhs.grid[0][2] + grid[2][1] * rhs.grid[1][2] + grid[2][2] * rhs.grid[2][2] + grid[2][3] * rhs.grid[3][2],
		grid[2][0] * rhs.grid[0][3] + grid[2][1] * rhs.grid[1][3] + grid[2][2] * rhs.grid[2][3] + grid[2][3] * rhs.grid[3][3],
		grid[3][0] * rhs.grid[0][0] + grid[3][1] * rhs.grid[1][0] + grid[3][2] * rhs.grid[2][0] + grid[3][3] * rhs.grid[3][0],
		grid[3][0] * rhs.grid[0][1] + grid[3][1] * rhs.grid[1][1] + grid[3][2] * rhs.grid[2][1] + grid[3][3] * rhs.grid[3][1],
		grid[3][0] * rhs.grid[0][2] + grid[3][1] * rhs.grid[1][2] + grid[3][2] * rhs.grid[2][2] + grid[3][3] * rhs.grid[3][2],
		grid[3][0] * rhs.grid[0][3] + grid[3][1] * rhs.grid[1][3] + grid[3][2] * rhs.grid[2][3] + grid[3][3] * rhs.grid[3][3]);

	return m;
}

Matrix Matrix::operator*(float f) const
{
	Matrix m(
		grid[0][0] * f, grid[0][1] * f, grid[0][2] * f, grid[0][3] * f,
		grid[1][0] * f, grid[1][1] * f, grid[1][2] * f, grid[1][3] * f,
		grid[2][0] * f, grid[2][1] * f, grid[2][2] * f, grid[2][3] * f,
		grid[3][0] * f, grid[3][1] * f, grid[3][2] * f, grid[3][3] * f);

	return m;
}

Matrix operator*(float f, const Matrix& lhs)
{
	Matrix m(
		lhs.grid[0][0] * f, lhs.grid[0][1] * f, lhs.grid[0][2] * f, lhs.grid[0][3] * f,
		lhs.grid[1][0] * f, lhs.grid[1][1] * f, lhs.grid[1][2] * f, lhs.grid[1][3] * f,
		lhs.grid[2][0] * f, lhs.grid[2][1] * f, lhs.grid[2][2] * f, lhs.grid[2][3] * f,
		lhs.grid[3][0] * f, lhs.grid[3][1] * f, lhs.grid[3][2] * f, lhs.grid[3][3] * f);

	return m;
}

Vector4 Matrix::operator*(const Vector2& v) const
{
	Vector4 vect(
		grid[0][0] * v.x + grid[0][1] * v.y + grid[0][3] * 1.0f,
		grid[1][0] * v.x + grid[1][1] * v.y + grid[1][3] * 1.0f,
		grid[2][0] * v.x + grid[2][1] * v.y + grid[2][3] * 1.0f,
		grid[3][0] * v.x + grid[3][1] * v.y + grid[3][3] * 1.0f);

	return vect;
}

Vector4 Matrix::operator*(const Vector4& v) const
{
	Vector4 vect(
		grid[0][0] * v.x + grid[0][1] * v.y + grid[0][2] * v.z + grid[0][3] * v.w,
		grid[1][0] * v.x + grid[1][1] * v.y + grid[1][2] * v.z + grid[1][3] * v.w,
		grid[2][0] * v.x + grid[2][1] * v.y + grid[2][2] * v.z + grid[2][3] * v.w,
		grid[3][0] * v.x + grid[3][1] * v.y + grid[3][2] * v.z + grid[3][3] * v.w);

	return vect;
}

Matrix Matrix::operator/(float f) const
{
	Matrix m(
		grid[0][0] / f, grid[0][1] / f, grid[0][2] / f, grid[0][3] / f,
		grid[1][0] / f, grid[1][1] / f, grid[1][2] / f, grid[1][3] / f,
		grid[2][0] / f, grid[2][1] / f, grid[2][2] / f, grid[2][3] / f,
		grid[3][0] / f, grid[3][1] / f, grid[3][2] / f, grid[3][3] / f);

	return m;
}

Matrix& Matrix::operator+=(const Matrix& rhs)
{
	*this = *this + rhs;

	return *this;
}

Matrix& Matrix::operator-=(const Matrix& rhs)
{
	*this = *this - rhs;

	return *this;
}

Matrix& Matrix::operator*=(const Matrix& rhs)
{
	*this = *this * rhs;

	return *this;
}

Matrix& Matrix::operator*=(float f)
{
	*this = *this * f;

	return *this;
}

Matrix& Matrix::operator/=(float f)
{
	*this = *this / f;

	return *this;
}

Matrix Matrix::Inverse(void) const
{
	float s0, s1, s2, s3, s4, s5, c0, c1, c2, c3, c4, c5;

	/* Determinants of 2x2 submatrices */
	s0 = grid[0][0] * grid[1][1] - grid[0][1] * grid[1][0];
	s1 = grid[0][0] * grid[1][2] - grid[0][2] * grid[1][0];
	s2 = grid[0][0] * grid[1][3] - grid[0][3] * grid[1][0];
	s3 = grid[0][1] * grid[1][2] - grid[0][2] * grid[1][1];
	s4 = grid[0][1] * grid[1][3] - grid[0][3] * grid[1][1];
	s5 = grid[0][2] * grid[1][3] - grid[0][3] * grid[1][2];
	c5 = grid[2][2] * grid[3][3] - grid[2][3] * grid[3][2];
	c4 = grid[2][1] * grid[3][3] - grid[2][3] * grid[3][1];
	c3 = grid[2][1] * grid[3][2] - grid[2][2] * grid[3][1];
	c2 = grid[2][0] * grid[3][3] - grid[2][3] * grid[3][0];
	c1 = grid[2][0] * grid[3][2] - grid[2][2] * grid[3][0];
	c0 = grid[2][0] * grid[3][1] - grid[2][1] * grid[3][0];

	float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

	Matrix adj(													// Index
		grid[1][1] * c5 - grid[1][2] * c4 + grid[1][3] * c3,	// 00
		-grid[0][1] * c5 + grid[0][2] * c4 - grid[0][3] * c3,	// 01
		grid[3][1] * s5 - grid[3][2] * s4 + grid[3][3] * s3,	// 02
		-grid[2][1] * s5 + grid[2][2] * s4 - grid[2][3] * s3,	// 03
		-grid[1][0] * c5 + grid[1][2] * c2 - grid[1][3] * c1,	// 10
		grid[0][0] * c5 - grid[0][2] * c2 + grid[0][3] * c1,	// 11
		-grid[3][0] * s5 + grid[3][2] * s2 - grid[3][3] * s1,	// 12
		grid[2][0] * s5 - grid[2][2] * s2 + grid[2][3] * s1,	// 13
		grid[1][0] * c4 - grid[1][1] * c2 + grid[1][3] * c0,	// 20
		-grid[0][0] * c4 + grid[0][1] * c2 - grid[0][3] * c0,	// 21
		grid[3][0] * s4 - grid[3][1] * s2 + grid[3][3] * s0,	// 22
		-grid[2][0] * s4 + grid[2][1] * s2 - grid[2][3] * s0,	// 23
		-grid[1][0] * c3 + grid[1][1] * c1 - grid[1][2] * c0,	// 30
		grid[0][0] * c3 - grid[0][1] * c1 + grid[0][2] * c0,	// 31
		-grid[3][0] * s3 + grid[3][1] * s1 - grid[3][2] * s0,	// 32
		grid[2][0] * s3 - grid[2][1] * s1 + grid[2][2] * s0);	// 33

	return (1 / det) * adj;
}

Matrix Matrix::Transpose(void) const
{
	Matrix m(
		grid[0][0], grid[1][0], grid[2][0], grid[3][0],
		grid[0][1], grid[1][1], grid[2][1], grid[3][1],
		grid[0][2], grid[1][2], grid[2][2], grid[3][2],
		grid[0][3], grid[1][3], grid[2][3], grid[3][3]);

	return m;
}

Matrix Matrix::Rotate(const Vector3& angle)
{
	Vector3 radians = angle * (PI * rcp180);

	Matrix m = Matrix::RotateZ(radians.z) * Matrix::RotateY(radians.y) * Matrix::RotateX(radians.x);

	return m;
}

Matrix Matrix::RotateX(float radians)	// pitch
{
	Matrix m(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(radians), -sin(radians), 0.0f,
		0.0f, sin(radians), cos(radians), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::RotateY(float radians)	// roll
{
	Matrix m(
		cos(radians), 0.0f, sin(radians), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(radians), 0.0f, cos(radians), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::RotateZ(float radians)	// yaw
{
	Matrix m(
		cos(radians), -sin(radians), 0.0f, 0.0f,
		sin(radians), cos(radians), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::Scale(const Vector3& scale)
{
	Matrix m(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::Scale(float x, float y, float z)
{
	Matrix m(
		x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::Translate(const Vector3& translate)
{
	Matrix m(
		1.0f, 0.0f, 0.0f, translate.x,
		0.0f, 1.0f, 0.0f, translate.y,
		0.0f, 0.0f, 1.0f, translate.z,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::Translate(float x, float y, float z)
{
	Matrix m(
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::Identity(void)
{
	Matrix m(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::LookAtLH(const Vector3& cameraPos, const Vector3& targetPoint, const Vector3& upVector)
{
#ifdef _DEBUG
	if (cameraPos == targetPoint)
	{
		fprintf(stderr, "Matrix::%s: target cannot be equal to camera position\n", __func__);
	}
#endif

	Vector3 w = targetPoint - cameraPos;
	w = w.Normalize();

	Vector3 u = upVector.Cross(w);
	u = u.Normalize();

	Vector3 v = w.Cross(u);

	Matrix m(
		u.x, v.x, w.x, -cameraPos.x,
		u.y, v.y, w.y, -cameraPos.y,
		u.z, v.z, w.z, -cameraPos.z,
		0.0f, 0.0f, 0.0f, 1.0f);

	return m;
}

Matrix Matrix::PerspectiveLH(uint32_t width, uint32_t height, float nearPlane, float farPlane)
{
#ifdef _DEBUG
	if (width == 0U || height == 0U)
	{
		fprintf(stderr, "Matrix::%s: width or height cannot be 0\n", __func__);
	}
	if (nearPlane >= farPlane)
	{
		fprintf(stderr, "Matrix::%s: nearPlane >= farPlane\n", __func__);
	}
#endif

	float fWidth = static_cast<float>(width);
	float fHeight = static_cast<float>(height);

	return Matrix(
		2.0f * nearPlane / fWidth, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f * nearPlane / fHeight, 0.0f, 0.0f,
		0.0f, 0.0f, farPlane / (farPlane - nearPlane), nearPlane * farPlane / (nearPlane - farPlane),
		0.0f, 0.0f, 1.0f, 0.0f);
}

void Matrix::Print(FILE* stream) const
{
	for (int line = 0; line < 4; ++line)
	{
		fprintf(stream, "%.3f\t%.3f\t%.3f\t%.3f\n", grid[line][0], grid[line][1], grid[line][2], grid[line][3]);
	}
	putc('\n', stream);
}

float Matrix::Determinant3x3(
	float f00, float f01, float f02,
	float f10, float f11, float f12,
	float f20, float f21, float f22)
{
	return (f00 * f11 * f22 + f01 * f12 * f20 + f02 * f10 * f21) - (f20 * f11 * f02 + f10 * f01 * f22 + f00 * f21 * f12);
}
