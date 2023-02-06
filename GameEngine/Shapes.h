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

#ifndef SHAPES_H
#define SHAPES_H

#include "Vector2.h"
#include "Vector3.h"
#include <string>

enum class ShapeType
{
	Point,
	Circle,
	AABB,
	OBB
};

class Shape
{
public:
	ShapeType m_type;
	std::string m_typeStr;
	Vector3 m_center;
	Vector3 m_previousCenter; // for calculating movement

protected:
	Shape(ShapeType type, const std::string& typeStr, const Vector3& center)
		: m_type(type)
		, m_typeStr(typeStr)
		, m_center(center)
		, m_previousCenter(center)
	{}
};

class Point : public Shape
{
public:
	Point(void)
		: Shape(ShapeType::Point, "Point", { 0.0f, 0.0f, 0.0f })
	{}

	Point(const Vector3& position)
		: Shape(ShapeType::Point, "Point", position)
	{}
};

class Circle : public Shape
{
public:
	Circle(const Vector3& center, float radius)
		: Shape(ShapeType::Circle, "Circle", center)
		, m_radius(radius)
	{}

	float m_radius;
};

class AABB : public Shape
{
public:
	AABB(const Vector3& center, const Vector2& halfExtents)
		: Shape(ShapeType::AABB, "AABB", center)
		, m_halfExtents(halfExtents)
	{}

	Vector2 m_halfExtents;
};

class OBB : public Shape
{
public:
	OBB(const Vector3& center, const Vector2& halfExtents, float angle)
		: Shape(ShapeType::OBB, "OBB", center)
		, m_halfExtents(halfExtents)
		, m_rotation(angle)
	{}

	Vector2 m_halfExtents;
	float m_rotation; // degrees clockwise
};

#endif
