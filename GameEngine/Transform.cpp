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

#include "Transform.h"
#include "JSONData.h"

Transform::Transform(void)
{}

Transform::Transform(const Vector3& position, const Vector3& scale)
	: m_position(position)
	, m_scale(scale)
{}

Transform::Transform(const Vector3& position, const Vector3& scale, const Vector3& rotation)
	: m_position(position)
	, m_scale(scale)
	, m_rotation(rotation)
{}

void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_rotation = rotation;
}

const Vector3& Transform::GetPosition(void) const
{
	return m_position;
}

const Vector3& Transform::GetScale(void) const
{
	return m_scale;
}

const Vector3& Transform::GetRotation(void) const
{
	return m_rotation;
}

void Transform::SetTransformMatrix(const Matrix& transform)
{
	m_transientTransform = transform;
}

const Matrix& Transform::GetTransformMatrix(void) const
{
	return m_transientTransform;
}

void Transform::Deserialize(const JSONData& data)
{
	data.GetVector3("position", m_position);
	data.GetVector3("scale", m_scale);
	data.GetVector3("rotation", m_rotation);
}
