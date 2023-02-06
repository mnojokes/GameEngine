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

#ifndef COLLISION_H
#define COLLISION_H

#include <cstdio>
#include "Shapes.h"

namespace Collision
{
	struct ShapeCollision
	{
		float m_time = -1.0f; // collision time (-1.0 means no collision)
		Vector3 m_normal; // collision normal
	};

	// CollisionEvent contains two ShapeCollision objects with collision adjustment data for this shape,
	// collidingShape contains collision data for the other shape assuming it did not move from its final position
	struct CollisionEvent
	{
		bool isCollision = false;
		ShapeCollision thisShape;
		ShapeCollision collidingShape;
	};

	// CollisionResult contains two collision events for shapes A and B respectively passed to any IsCollision method.
	// Both events contain collision data for each shape considering the other shape was in its final position and did not move
	struct CollisionResult
	{
		CollisionEvent A;
		CollisionEvent B;
	};

	//// SHAPES
	CollisionResult IsCollision(const Shape& a, const Shape& b);

	//// POINT
	// Point & point
	CollisionResult IsCollision(const Point& a, const Point& b);
	// Point & circle
	CollisionResult IsCollision(const Point& a, const Circle& b);
	// Point & AABB
	CollisionResult IsCollision(const Point& a, const AABB& b);
	// Point & OBB
	CollisionResult IsCollision(const Point& a, const OBB& b);

	//// CIRCLE
	// Circle & point
	CollisionResult IsCollision(const Circle& a, const Point& b);
	// Circle & circle
	CollisionResult IsCollision(const Circle& a, const Circle& b);
	// Circle & AABB
	CollisionResult IsCollision(const Circle& a, const AABB& b);
	// Circle & OBB
	CollisionResult IsCollision(const Circle& a, const OBB& b);

	//// AABB
	// AABB & point
	CollisionResult IsCollision(const AABB& a, const Point& b);
	// AABB & circle
	CollisionResult IsCollision(const AABB& a, const Circle& b);
	// AABB & AABB
	CollisionResult IsCollision(const AABB& a, const AABB& b);
	// AABB & OBB
	CollisionResult IsCollision(const AABB& a, const OBB& b);
	
	//// OBB
	// OBB & point
	CollisionResult IsCollision(const OBB& a, const Point& b);
	// OBB & circle
	CollisionResult IsCollision(const OBB& a, const Circle& b);
	// OBB & AABB
	CollisionResult IsCollision(const OBB& a, const AABB& b);
	// OBB & OBB
	CollisionResult IsCollision(const OBB& a, const OBB& b);
};

#endif
