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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Matrix.h"

class JSONData;

class Transform
{
public:
	Transform(void);
	Transform(const Vector3& position, const Vector3& scale);
	Transform(const Vector3& position, const Vector3& scale, const Vector3& rotation);

	void SetPosition(const Vector3& position);
	const Vector3& GetPosition(void) const;
	void SetScale(const Vector3& scale);
	const Vector3& GetScale(void) const;
	void SetRotation(const Vector3& rotation); // degrees clockwise
	const Vector3& GetRotation(void) const; // degrees clockwise

	void SetTransformMatrix(const Matrix& transform);
	const Matrix& GetTransformMatrix(void) const;

	void Deserialize(const JSONData& data);

private:
	Vector3 m_position;
	Vector3 m_scale;
	Vector3 m_rotation; // degrees clockwise
	Matrix m_transientTransform;
};

#endif
