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

#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "Component.h"
#include "Vector3.h"

class Shape;

class PhysicsComponent : public Component
{
public:
	static void Register(void);

	PhysicsComponent(void);
	PhysicsComponent(uint64_t id, GameObject* owner);
	PhysicsComponent(const PhysicsComponent& rhs) = default;
	~PhysicsComponent(void);

	Shape& GetColliderShape(void) const;
	float GetColliderWeight(void) const;
	void SetColliderWeight(float weight);

	const std::vector<Vector3>& GetCollisions(void) const;
	void AddCollision(const Vector3& normal);
	void ResetCollisions(void);

	const Vector3& GetVelocity(void) const;
	void SetVelocity(const Vector3& velocity);
	void ResetVelocity(void);

	const Vector3& GetLastPosition(void) const;

	void Deserialize(const JSONData& source) override;

	Component* Clone(void) const override;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

	void NotifyCollision(void);

private:
	Shape* m_collider;
	float m_colliderWeight; // heavier objects do not move when colliding with lighter objects, 0.0 means the object will not collide (ghost)
	Vector3 m_velocity;

	std::vector<Vector3> m_collisions; // stores collision normals of colliding shapes
};

#endif