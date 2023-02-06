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

#include "Collision.h"
#include "MathConstants.h"
#include "Algebra.h"
#include "Macros.h"
#include <cmath>

namespace
{
	// Indices for when Circle points are stored in array
	const int CIRCLE_TOP = 0;
	const int CIRCLE_RIGHT = 1;
	const int CIRCLE_BOTTOM = 2;
	const int CIRCLE_LEFT = 3;

	// Indices for when AABB points are stored in array
	const int AABB_TOP_LEFT = 0;
	const int AABB_TOP_RIGHT = 1;
	const int AABB_BOT_RIGHT = 2;
	const int AABB_BOT_LEFT = 3;

	const int NUM_POINTS = 4;

	const Vector3 aabbPointNormals[NUM_POINTS] = {
		Vector3(-1.0f, 1.0f, 0.0f).Normalize(),
		Vector3(1.0f, 1.0f, 0.0f).Normalize(),
		Vector3(1.0f, -1.0f, 0.0f).Normalize(),
		Vector3(-1.0f, -1.0f, 0.0f).Normalize()
	};

	Collision::ShapeCollision GetCircleCollisionAdjustmentData(
		const Circle& adjustable, const Circle& stationary, float distanceSq, float sumRadiiSq)
	{
		Vector3 aVelocity = adjustable.m_center - adjustable.m_previousCenter;
		float aMoveDistance = aVelocity.Magnitude();

		// Entire range (line) of a's center's movement
		Math::Line aMovementLine = Math::LineFromTwoPoints(adjustable.m_previousCenter, adjustable.m_center);
		// Distance from b's center to intersecting a's movement line
		float distanceBCenterToLine = Math::DistancePointToLine(aMovementLine, stationary.m_center);
		float distanceBCenterToLineSq = POW2(distanceBCenterToLine);
		// Distances from intersection point
		float distIntersectToACenter = sqrt(distanceSq - distanceBCenterToLineSq);
		float distIntersectToNewACenter = sqrt(sumRadiiSq - distanceBCenterToLineSq);
		// Difference between two distances
		float diff = distIntersectToNewACenter - distIntersectToACenter;

		float collisionTime = 1.0f - diff / aMoveDistance;
		Vector3 adjustedCenter = adjustable.m_previousCenter + aVelocity * collisionTime;
		Vector3 differenceStationaryToAdjustedCenter = stationary.m_center - adjustedCenter;
		Vector3 collisionNormal = differenceStationaryToAdjustedCenter.Normalize();

		return { collisionTime, collisionNormal };
	}

	bool IsShapeMoved(const Shape& shape)
	{
		return shape.m_center != shape.m_previousCenter;
	}

	bool IsAnyShapeMoved(const Shape& a, const Shape& b)
	{
		return IsShapeMoved(a) || IsShapeMoved(b);
	}

	bool IsDeeperCollision(const Collision::CollisionEvent& cur, const Collision::CollisionEvent& old)
	{
		if (cur.isCollision)
		{
			if (!old.isCollision || cur.thisShape.m_time < old.thisShape.m_time)
			{
				return true;
			}
		}

		return false;
	}

	bool IsPotentiallyColliding(const Circle& a, const AABB& b)
	{
		float distX = fabs(b.m_center.x - a.m_center.x);
		float halfExtentsRadiusX = a.m_radius + b.m_halfExtents.x;

		if (distX <= halfExtentsRadiusX)
		{
			float distY = fabs(b.m_center.y - a.m_center.y);
			float halfExtentsRadiusY = a.m_radius + b.m_halfExtents.y;
			if (distY <= halfExtentsRadiusY)
			{
				return true;
			}
		}

		return false;
	}

	bool IsPotentiallyColliding(const AABB& a, const AABB& b)
	{
		float distX = fabs(b.m_center.x - a.m_center.x);
		float halfExtentsX = a.m_halfExtents.x + b.m_halfExtents.x;
		
		if (distX <= halfExtentsX)
		{
			float distY = fabs(b.m_center.y - a.m_center.y);
			float halfExtentsY = a.m_halfExtents.y + b.m_halfExtents.y;
			if (distY <= halfExtentsY)
			{
				return true;
			}
		}

		return false;
	}

	bool IsPointWithinAABB(const Vector3& point, const AABB& aabb)
	{
		Vector3 aabbBotLeft = { aabb.m_center.x - aabb.m_halfExtents.x, aabb.m_center.y - aabb.m_halfExtents.y, 0.0f };
		Vector3 aabbTopRight = { aabb.m_center.x + aabb.m_halfExtents.x, aabb.m_center.y + aabb.m_halfExtents.y, 0.0f };

		bool res = false;

		if (aabbBotLeft.x <= point.x && point.x <= aabbTopRight.x)
		{
			if (aabbBotLeft.y <= point.y && point.y <= aabbTopRight.y)
			{
				res = true;
			}
		}

		return res;
	}

	void GetShapePointsAndVelocity(const Circle& circle, Point pointArray[], Vector3& velocity)
	{
#ifdef _DEBUG
		if (pointArray == nullptr)
		{
			fprintf(stderr, "Collision::%s: Circle pointArray was nullptr\n", __func__);
			return;
		}
#endif

		velocity = circle.m_center - circle.m_previousCenter;

		pointArray[CIRCLE_TOP].m_center = { circle.m_center.x, circle.m_center.y + circle.m_radius, circle.m_center.z };
		pointArray[CIRCLE_TOP].m_previousCenter = pointArray[CIRCLE_TOP].m_center - velocity;
		pointArray[CIRCLE_RIGHT].m_center = { circle.m_center.x + circle.m_radius, circle.m_center.y, circle.m_center.z };
		pointArray[CIRCLE_RIGHT].m_previousCenter = pointArray[CIRCLE_RIGHT].m_center - velocity;
		pointArray[CIRCLE_BOTTOM].m_center = { circle.m_center.x, circle.m_center.y - circle.m_radius, circle.m_center.z };
		pointArray[CIRCLE_BOTTOM].m_previousCenter = pointArray[CIRCLE_BOTTOM].m_center - velocity;
		pointArray[CIRCLE_LEFT].m_center = { circle.m_center.x - circle.m_radius, circle.m_center.y, circle.m_center.z };
		pointArray[CIRCLE_LEFT].m_previousCenter = pointArray[CIRCLE_LEFT].m_center - velocity;
	}

	void GetShapePointsAndVelocity(const AABB& aabb, Point pointArray[], Vector3& velocity)
	{
		// TODO fix this
		//if (ARRAY_SIZE(pointArray) != NUM_POINTS)
		//{
		//	fprintf(stderr, "Collision::%s: AABB pointArray size is not %d\n", __func__, NUM_POINTS);
		//	return;
		//}

		velocity = aabb.m_center - aabb.m_previousCenter;

		pointArray[AABB_TOP_LEFT].m_center = { aabb.m_center.x - aabb.m_halfExtents.x, aabb.m_center.y + aabb.m_halfExtents.y, aabb.m_center.z };
		pointArray[AABB_TOP_LEFT].m_previousCenter = pointArray[AABB_TOP_LEFT].m_center - velocity;
		pointArray[AABB_TOP_RIGHT].m_center = { aabb.m_center.x + aabb.m_halfExtents.x, aabb.m_center.y + aabb.m_halfExtents.y, aabb.m_center.z };
		pointArray[AABB_TOP_RIGHT].m_previousCenter = pointArray[AABB_TOP_RIGHT].m_center - velocity;
		pointArray[AABB_BOT_LEFT].m_center = { aabb.m_center.x - aabb.m_halfExtents.x, aabb.m_center.y - aabb.m_halfExtents.y, aabb.m_center.z };
		pointArray[AABB_BOT_LEFT].m_previousCenter = pointArray[AABB_BOT_LEFT].m_center - velocity;
		pointArray[AABB_BOT_RIGHT].m_center = { aabb.m_center.x + aabb.m_halfExtents.x, aabb.m_center.y - aabb.m_halfExtents.y, aabb.m_center.z };
		pointArray[AABB_BOT_RIGHT].m_previousCenter = pointArray[AABB_BOT_RIGHT].m_center - velocity;
	}

	Collision::ShapeCollision LineIntersection(const Math::Line& a, const Math::Line& b)
	{
		// k0x + b0 = k1x + b1
		float left = 0.0f;
		float right = 0.0f;
		if (a.m_bound0.x == a.m_bound1.x) // edge case: a vertical
		{
			if (b.m_bound0.x == b.m_bound1.x) // edge case: b vertical
			{
				left = 0.0f;
				right = b.m_bound0.x - a.m_bound0.x;
			}
			else // b normal
			{
				left = 1.0f;
				right = a.m_bound0.x;
			}
		}
		else // a normal
		{
			if (b.m_bound0.x == b.m_bound1.x) // edge case: b vertical
			{
				left = 1.0f;
				right = b.m_bound0.x;
			}
			else // average case: both a and b normal
			{
				left = a.k - b.k;
				right = b.b - a.b;
			}
		}

		if (left != 0.0f && left != 1.0f)
		{
			right /= left;
			left = 1.0f;
		}

		if (left == 0.0f && right != 0.0f)
		{ // lines do not intersect
			return {};
		}
		else if (left == 0.0f && right == 0.0f)
		{ // lines match - infinite intersections
			// Find local min and max
			const Math::Line* lines[] = { &a, &a, &b, &b };
			const Math::Line* minLine = nullptr;
			const Math::Line* maxLine = nullptr;

			Vector3 points[NUM_POINTS] = { a.m_bound0, a.m_bound1, b.m_bound0, b.m_bound1 };
			Vector3 min = points[0];
			for (int i = 1; i < NUM_POINTS; ++i)
			{
				if (points[i].x < min.x)
				{
					min = points[i];
					minLine = lines[i];

				}
				else if (points[i].x == min.x)
				{
					if (points[i].y < min.y)
					{
						min = points[i];
						minLine = lines[i];
					}
				}
			}
			Vector3 max = points[0];
			for (int i = 1; i < NUM_POINTS; ++i)
			{
				if (points[i].x > max.x)
				{
					max = points[i];
					maxLine = lines[i];
				}
				else if (points[i].x == max.x)
				{
					if (points[i].y > max.y)
					{
						max = points[i];
						maxLine = lines[i];
					}
				}
			}
			
			// Find lengths and range between min & max
			float aLen = Math::LineLength(a);
			float bLen = Math::LineLength(b);
			float range = Math::LineLength(min, max);

			// (AB + CD) - (AD)
			float intersectionRange = aLen + bLen - range;
			float intersectionTime = 1.0f - intersectionRange / bLen;

			Vector3 intersectionNormal;

			if (min == a.m_bound0 || min == a.m_bound1)
			{
				intersectionNormal = min - max;
			}
			else
			{
				intersectionNormal = max - min;
			}

			return { intersectionTime, intersectionNormal.Normalize() };
		}
		
		// (y = k0x + b0)
		const Math::Line* nonVerticalLine = &a;
		if (a.m_bound0.x == a.m_bound1.x)
		{
			nonVerticalLine = &b;
		}

		Vector3 intersectPoint = nonVerticalLine->F(right);
		Vector3 bVect = b.m_bound1 - b.m_bound0;
		Vector3 b0ToIntersect = intersectPoint - b.m_bound0;
		Vector3 b1ToIntersect = intersectPoint - b.m_bound1;
		float bLenSq = bVect.MagnitudeSq();
		float b0ToIntersectDistSq = b0ToIntersect.MagnitudeSq();
		float b1ToIntersectDistSq = b1ToIntersect.MagnitudeSq();
		//float bLen = Math::LineLength(b);

		if (b0ToIntersectDistSq > bLenSq || b1ToIntersectDistSq > bLenSq)
		{ // intersection out of b bounds
			return {};
		}

		float intersectTime = sqrt(b0ToIntersectDistSq) / sqrt(bLenSq);
		Vector3 intersectDirOnB = intersectPoint - b.m_bound0;

		return { intersectTime, intersectDirOnB.Normalize() };
	}

	Vector3 GetAABBLineNormal(const Math::Line& line, const Vector3& aabbCenter)
	{
		Vector3 bound0ToCenter = line.m_bound0 - aabbCenter;
		Vector3 bound1ToCenter = line.m_bound1 - aabbCenter;
		Vector3 ret = (bound0ToCenter + bound1ToCenter);

		return ret.Normalize();
	}

	Vector2 RotateVector(const Vector2& vec, float angle)
	{
		float rad = angle * (PI * rcp180);

		float x1 = vec.x * cos(rad);
		float x2 = vec.y * sin(rad);
		float x = x1 + x2;

		float y1 = vec.x * -sin(rad);
		float y2 = vec.y * cos(rad);
		float y = y1 + y2;


		Vector2 retVec({ vec.x * cos(rad) + vec.y * sin(rad), vec.x * -sin(rad) + vec.y * cos(rad) });

		return retVec;
	}
};

namespace Collision
{
	// Checks two shapes for collisions and provides adjustment data to move one of the shapes out of collision.
	// If both shapes moved, adjustments for both are calculated assuming the other shape stays in its final position.
	CollisionResult IsCollision(const Shape& a, const Shape& b)
	{
		if (!IsAnyShapeMoved(a, b))
		{
			return {};
		}

		CollisionResult res = {};

		switch (a.m_type)
		{
		case ShapeType::Point:
			switch (b.m_type)
			{
			case ShapeType::Point:
				res = IsCollision(static_cast<const Point&>(a), static_cast<const Point&>(b));
				break;
			case ShapeType::Circle:
				res = IsCollision(static_cast<const Point&>(a), static_cast<const Circle&>(b));
				break;
			case ShapeType::AABB:
				res = IsCollision(static_cast<const Point&>(a), static_cast<const AABB&>(b));
				break;
			case ShapeType::OBB:
				res = IsCollision(static_cast<const Point&>(a), static_cast<const OBB&>(b));
				break;
			default:
				break;
			}
			break;
		case ShapeType::Circle:
			switch (b.m_type)
			{
			case ShapeType::Point:
				res = IsCollision(static_cast<const Circle&>(a), static_cast<const Point&>(b));
				break;
			case ShapeType::Circle:
				res = IsCollision(static_cast<const Circle&>(a), static_cast<const Circle&>(b));
				break;
			case ShapeType::AABB:
				res = IsCollision(static_cast<const Circle&>(a), static_cast<const AABB&>(b));
				break;
			case ShapeType::OBB:
				res = IsCollision(static_cast<const Circle&>(a), static_cast<const OBB&>(b));
				break;
			default:
				break;
			}
			break;
		case ShapeType::AABB:
			switch (b.m_type)
			{
			case ShapeType::Point:
				res = IsCollision(static_cast<const AABB&>(a), static_cast<const Point&>(b));
				break;
			case ShapeType::Circle:
				res = IsCollision(static_cast<const AABB&>(a), static_cast<const Circle&>(b));
				break;
			case ShapeType::AABB:
				res = IsCollision(static_cast<const AABB&>(a), static_cast<const AABB&>(b));
				break;
			case ShapeType::OBB:
				res = IsCollision(static_cast<const AABB&>(a), static_cast<const OBB&>(b));
				break;
			default:
				break;
			}
			break;
		case ShapeType::OBB:
			switch (b.m_type)
			{
			case ShapeType::Point:
				res = IsCollision(static_cast<const OBB&>(a), static_cast<const Point&>(b));
				break;
			case ShapeType::Circle:
				res = IsCollision(static_cast<const OBB&>(a), static_cast<const Circle&>(b));
				break;
			case ShapeType::AABB:
				res = IsCollision(static_cast<const OBB&>(a), static_cast<const AABB&>(b));
				break;
			case ShapeType::OBB:
				res = IsCollision(static_cast<const OBB&>(a), static_cast<const OBB&>(b));
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		return res;
	}

	CollisionResult IsCollision(const Point& a, const Point& b)
	{
		CollisionResult res = {};

		if (a.m_center == b.m_center)
		{
			ShapeCollision aCol, bCol;
			res.A.thisShape.m_time = a.m_center == a.m_previousCenter ? 0.0f : 1.0f;
			res.A.collidingShape.m_time = 1.0f;
			res.B.thisShape.m_time = b.m_center == b.m_previousCenter ? 0.0f : 1.0f;
			res.B.collidingShape.m_time = 1.0f;
		}

		return res;
	}

	CollisionResult IsCollision(const Point& a, const Circle& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const Point& a, const AABB& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const Point& a, const OBB& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const Circle& a, const Point& b)
	{
		CollisionResult endResult = {};

		//Find distances between center and point squared
		Vector3 distance(b.m_center - a.m_center);
		float distanceABSq = POW2(distance.x) + POW2(distance.y);
		float aRadiusSq = POW2(a.m_radius);
		if (distanceABSq <= aRadiusSq)
		{
			if (IsShapeMoved(a))
			{
				Vector3 aVelocity = a.m_center - a.m_previousCenter;
				float aMoveDistance = aVelocity.Magnitude();
			
				// Entire range (line) of a's center's movement
				Math::Line aMovementLine = Math::LineFromTwoPoints(a.m_previousCenter, a.m_center);
				// Distance from b to intersecting a's movement line
				float distanceToLine = Math::DistancePointToLine(aMovementLine, b.m_center);
				float distanceToLineSq = POW2(distanceToLine);
				// Distances from intersection point
				float distIntersectToCenterSq = fabs(distanceABSq - distanceToLineSq);
				float distIntersectToCenter = sqrt(distIntersectToCenterSq);
				float distIntersectToNewCenterSq = fabs(aRadiusSq - distanceToLineSq);
				float distIntersectToNewCenter = sqrt(distIntersectToNewCenterSq);
				// Difference between two distances
				float diff = fabs(distIntersectToNewCenter - distIntersectToCenter);
				float collisionTime = 1.0f - diff / aMoveDistance;

				endResult.A.isCollision = true;
				endResult.A.thisShape.m_time = collisionTime;
				endResult.A.collidingShape.m_time = 1.0f;

				Vector3 aCenterAdjusted = a.m_previousCenter + aVelocity * collisionTime;
				Vector3 differenceBToACenterAdjusted = b.m_center - aCenterAdjusted;
				endResult.A.thisShape.m_normal = differenceBToACenterAdjusted.Normalize();
				endResult.A.collidingShape.m_normal = -endResult.A.thisShape.m_normal;
			}
			if (IsShapeMoved(b))
			{
				Vector3 bVelocity = b.m_center - b.m_previousCenter;
				float bMoveDistance = bVelocity.Magnitude();

				// Entire range (line) of b's movement
				Math::Line bMovementLine = Math::LineFromTwoPoints(b.m_previousCenter, b.m_center);
				// Distance from b to intersecting a's movement line
				float distanceToLine = Math::DistancePointToLine(bMovementLine, a.m_center);
				float distanceToLineSq = POW2(distanceToLine);
				// Distance from intersection point to b
				float distIntersectToBSq = fabs(distanceABSq - distanceToLineSq);
				float distIntersectToB = sqrt(distIntersectToBSq);
				float distIntersectToNewBSq = fabs(aRadiusSq - distanceToLineSq);
				float distIntersectToNewB = sqrt(distIntersectToNewBSq);
				// Difference between two distances
				float diff = fabs(distIntersectToNewB - distIntersectToB);
				float collisionTime = 1.0f - diff / bMoveDistance;

				endResult.B.isCollision = true;
				endResult.B.thisShape.m_time = collisionTime;
				endResult.B.collidingShape.m_time = 1.0f;

				Vector3 bAdjusted = b.m_previousCenter + bVelocity * collisionTime;
				Vector3 differenceAToBAdjusted = a.m_center - bAdjusted;

				endResult.B.thisShape.m_normal = differenceAToBAdjusted.Normalize();
				endResult.B.collidingShape.m_normal = -endResult.B.thisShape.m_normal;
			}
		}

		return endResult;
	}

	CollisionResult IsCollision(const Circle& a, const Circle& b)
	{
		CollisionResult endResult = {};

		//Find distances between both circles' centers squared
		Vector3 distance(b.m_center - a.m_center);
		float distanceABSq = POW2(distance.x) + POW2(distance.y);
		float sumRadiiSq = POW2(a.m_radius + b.m_radius);
		if (distanceABSq <= sumRadiiSq)
		{
			if (a.m_center != a.m_previousCenter)
			{
				endResult.A.thisShape = GetCircleCollisionAdjustmentData(a, b, distanceABSq, sumRadiiSq);
				endResult.A.collidingShape = { 1.0f, -endResult.A.thisShape.m_normal };
			}
			if (b.m_center != b.m_previousCenter)
			{
				endResult.B.thisShape = GetCircleCollisionAdjustmentData(b, a, distanceABSq, sumRadiiSq);
				endResult.B.collidingShape = { 1.0f, -endResult.B.thisShape.m_normal };;
			}
		}

		return endResult;
	}

	CollisionResult IsCollision(const Circle& a, const AABB& b)
	{
		if (!IsPotentiallyColliding(a, b))
		{
			return {};
		}

		CollisionResult endResult = {};

		Point aPoints[NUM_POINTS];
		Vector3 aVelocity;
		GetShapePointsAndVelocity(a, aPoints, aVelocity);

		Point bPoints[NUM_POINTS];
		Vector3 bVelocity;
		GetShapePointsAndVelocity(b, bPoints, bVelocity);

		CollisionResult collisions[] = {
			IsCollision(aPoints[CIRCLE_TOP], b),
			IsCollision(aPoints[CIRCLE_RIGHT], b),
			IsCollision(aPoints[CIRCLE_BOTTOM], b),
			IsCollision(aPoints[CIRCLE_LEFT], b),
			IsCollision(a, bPoints[AABB_TOP_LEFT]),
			IsCollision(a, bPoints[AABB_TOP_RIGHT]),
			IsCollision(a, bPoints[AABB_BOT_RIGHT]),
			IsCollision(a, bPoints[AABB_BOT_LEFT])
		};

		int bCollisionIndex = 0;

		for (int i = 0; i < 8; ++i)
		{ // find deepest collision for each shape
			if (IsDeeperCollision(collisions[i].A, endResult.A))
			{
				endResult.A = collisions[i].A;
			}
			if (IsDeeperCollision(collisions[i].B, endResult.B))
			{
				endResult.B = collisions[i].B;
				bCollisionIndex = i;
			}
		}

		bCollisionIndex -= NUM_POINTS;
		if (bCollisionIndex >= 0)
		{
			// assign a correct AABB corner normal to collision
			endResult.B.thisShape.m_normal = aabbPointNormals[bCollisionIndex];
		}



		return endResult;
	}

	CollisionResult IsCollision(const Circle& a, const OBB& b)
	{
		//// Use b as an AABB
		//AABB bAABB(b.m_center, b.m_halfExtents);

		//// Find vector of a center relative to b center
		//Vector2 aToB = { a.m_center.x - b.m_center.x, a.m_center.y - b.m_center.y };
		//Circle aRot(RotateVector(aToB, -b.m_rotation) + b.m_center, a.m_radius);

		//return IsCollision(aRot, bAABB);

		return {};
	}

	CollisionResult IsCollision(const AABB& a, const Point& b)
	{
		CollisionResult endResult = {};

		Point aPoints[NUM_POINTS];
		Vector3 aVelocity;
		GetShapePointsAndVelocity(a, aPoints, aVelocity);

		Vector3 bVelocity = b.m_center - b.m_previousCenter;

		if (IsPointWithinAABB(b.m_center, a))
		{
			if (IsShapeMoved(a))
			{
				// Create a lines assuming it did not move
				Math::Line aLines[] = {
				Math::LineFromTwoPoints(aPoints[AABB_TOP_LEFT].m_previousCenter, aPoints[AABB_TOP_RIGHT].m_previousCenter),
				Math::LineFromTwoPoints(aPoints[AABB_TOP_RIGHT].m_previousCenter, aPoints[AABB_BOT_RIGHT].m_previousCenter),
				Math::LineFromTwoPoints(aPoints[AABB_BOT_RIGHT].m_previousCenter, aPoints[AABB_BOT_LEFT].m_previousCenter),
				Math::LineFromTwoPoints(aPoints[AABB_BOT_LEFT].m_previousCenter, aPoints[AABB_TOP_LEFT].m_previousCenter)
				};

				// Move b relative to a
				Vector3 newBCenter = -aVelocity + b.m_center;
				Math::Line bMovt = Math::LineFromTwoPoints(b.m_center, newBCenter);

				ShapeCollision collision = {};
				for (int i = 0; i < NUM_POINTS; ++i)
				{
					ShapeCollision cur = LineIntersection(aLines[i], bMovt);
					if (cur.m_time != -1.0f)
					{
						collision.m_time = cur.m_time;
						collision.m_normal += GetAABBLineNormal(aLines[i], a.m_previousCenter);
						endResult.A.collidingShape.m_normal = cur.m_normal;
					}
				}

				endResult.A.isCollision = true;
				endResult.A.thisShape = collision;
				endResult.A.collidingShape.m_time = 1.0f;
			}
			if (IsShapeMoved(b))
			{
				Math::Line aLines[] = {
				Math::LineFromTwoPoints(aPoints[AABB_TOP_LEFT].m_center, aPoints[AABB_TOP_RIGHT].m_center),
				Math::LineFromTwoPoints(aPoints[AABB_TOP_RIGHT].m_center, aPoints[AABB_BOT_RIGHT].m_center),
				Math::LineFromTwoPoints(aPoints[AABB_BOT_RIGHT].m_center, aPoints[AABB_BOT_LEFT].m_center),
				Math::LineFromTwoPoints(aPoints[AABB_BOT_LEFT].m_center, aPoints[AABB_TOP_LEFT].m_center)
				};

				Math::Line bMovt = Math::LineFromTwoPoints(b.m_previousCenter, b.m_center);

				ShapeCollision collision = {};
				for (int i = 0; i < NUM_POINTS; ++i)
				{
					ShapeCollision cur = LineIntersection(aLines[i], bMovt);
					if (cur.m_time != -1.0f)
					{
						collision.m_time = cur.m_time;
						collision.m_normal = cur.m_normal;
						endResult.B.collidingShape.m_normal += GetAABBLineNormal(aLines[i], a.m_center);
					}
				}

				endResult.B.isCollision = true;
				endResult.B.thisShape = collision;
				endResult.B.collidingShape.m_time = 1.0f;
			}
		}

		return endResult;
	}

	CollisionResult IsCollision(const AABB& a, const Circle& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const AABB& a, const AABB& b)
	{
		if (!IsPotentiallyColliding(a, b))
		{
			return {};
		}

		CollisionResult endResult = {};

		Point aPoints[NUM_POINTS];
		Vector3 aVelocity;
		GetShapePointsAndVelocity(a, aPoints, aVelocity);

		Point bPoints[NUM_POINTS];
		Vector3 bVelocity;
		GetShapePointsAndVelocity(b, bPoints, bVelocity);

		CollisionResult collisions[] = {
			IsCollision(aPoints[AABB_TOP_LEFT], b),
			IsCollision(aPoints[AABB_TOP_RIGHT], b),
			IsCollision(aPoints[AABB_BOT_RIGHT], b),
			IsCollision(aPoints[AABB_BOT_LEFT], b),
			IsCollision(a, bPoints[AABB_TOP_LEFT]),
			IsCollision(a, bPoints[AABB_TOP_RIGHT]),
			IsCollision(a, bPoints[AABB_BOT_RIGHT]),
			IsCollision(a, bPoints[AABB_BOT_LEFT]),
		};

		for (int i = 0; i < 8; ++i)
		{
			// find deepest collisions for both shapes
			// assign inverted normals because colliding shapes have correct AABB normals
			if (IsDeeperCollision(collisions[i].A, endResult.A))
			{
				endResult.A = collisions[i].A;
				endResult.A.thisShape.m_normal = -endResult.A.collidingShape.m_normal;
			}
			if (IsDeeperCollision(collisions[i].B, endResult.B))
			{
				endResult.B = collisions[i].B;
				endResult.B.thisShape.m_normal = -endResult.B.collidingShape.m_normal;
			}
		}

		return endResult;
	}

	CollisionResult IsCollision(const AABB& a, const OBB& b)
	{
		// The code below will return incorrect results if used
		//Vector3 aTopLeft = { a.m_center.x - a.m_halfExtents.x, a.m_center.y + a.m_halfExtents.y };
		//Vector3 aTopRight = { a.m_center.x + a.m_halfExtents.x, a.m_center.y + a.m_halfExtents.y };
		//Vector3 aBotLeft = { a.m_center.x - a.m_halfExtents.x, a.m_center.y - a.m_halfExtents.y };
		//Vector3 aBotRight = { a.m_center.x + a.m_halfExtents.x, a.m_center.y - a.m_halfExtents.y };

		//CollisionResult res[8];
		//res[0] = IsCollision(b, aTopLeft);
		//res[1] = IsCollision(b, aTopRight);
		//res[2] = IsCollision(b, aBotLeft);
		//res[3] = IsCollision(b, aBotRight);

		//for (int i = 0; i < 4; ++i)
		//{
		//	if (res[i].A.m_time != -1.0f)
		//	{
		//		return res[i];
		//	}
		//}

		//Vector3 bTopLeft = RotateVector({ -b.m_halfExtents.x, b.m_halfExtents.y }, b.m_rotation) + b.m_center;
		//Vector3 bTopRight = RotateVector({ b.m_halfExtents.x, b.m_halfExtents.y }, b.m_rotation) + b.m_center;
		//Vector3 bBotLeft = RotateVector({ -b.m_halfExtents.x, -b.m_halfExtents.y }, b.m_rotation) + b.m_center;
		//Vector3 bBotRight = RotateVector({ b.m_halfExtents.x, -b.m_halfExtents.y }, b.m_rotation) + b.m_center;

		//res[0] = IsCollision(a, bTopLeft);
		//res[1] = IsCollision(a, bTopRight);
		//res[2] = IsCollision(a, bBotLeft);
		//res[3] = IsCollision(a, bBotRight);

		//for (int i = 0; i < 4; ++i)
		//{
		//	if (res[i].A.m_time != -1.0f)
		//	{
		//		return res[i];
		//	}
		//}

		return {};
	}

	CollisionResult IsCollision(const OBB& a, const Point& b)
	{
		// Use a as an AABB
		AABB aAABB(a.m_center, a.m_halfExtents);

		// Rotate and offset b so it's aligned with aAABB
		Vector3 bRot = RotateVector(b.m_center - a.m_center, -a.m_rotation) + a.m_center;

		return IsCollision(aAABB, bRot);
	}

	CollisionResult IsCollision(const OBB& a, const Circle& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const OBB& a, const AABB& b)
	{
		CollisionResult res = IsCollision(b, a);
		return { res.B, res.A };
	}

	CollisionResult IsCollision(const OBB& a, const OBB& b)
	{
		CollisionResult res = {};

		// The code below will return incorrect results if used
		//// Use a as an AABB, rotate b accordingly
		//AABB aAABB = { a.m_center, a.m_halfExtents };

		//Vector3 newBCenter(RotateVector(b.m_center - a.m_center, b.m_rotation - a.m_rotation) + a.m_center);
		//float newBRotation = b.m_rotation - a.m_rotation;
		//OBB bRot(newBCenter, b.m_halfExtents, newBRotation);

		//CollisionResult res = IsCollision(aAABB, bRot);
		//if (res.A.m_time != -1.0f)
		//{
		//	return res;
		//}

		//// Use b as an AABB, rotate a accordingly
		//AABB bAABB = { b.m_center, b.m_halfExtents };

		//Vector3 newACenter(RotateVector(a.m_center - b.m_center, a.m_rotation - b.m_rotation) + b.m_center);
		//float newARotation = a.m_rotation - b.m_rotation;
		//OBB aRot(newACenter, a.m_halfExtents, newARotation);

		//res = IsCollision(bAABB, aRot);

		return res;
	}
}
