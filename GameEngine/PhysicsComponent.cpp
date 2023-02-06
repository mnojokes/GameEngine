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

#include "PhysicsComponent.h"
#include "GameObjectFactory.h"
#include "JSONData.h"
#include "SceneComponent.h"
#include "Shapes.h"

void PhysicsComponent::Register(void)
{
	GameObjectFactory::Get()->RegisterComponent<PhysicsComponent>();
}

PhysicsComponent::PhysicsComponent(void)
	: Component(0U, nullptr)
	, m_collider(nullptr)
	, m_colliderWeight(0.0f)
{}

PhysicsComponent::PhysicsComponent(uint64_t id, GameObject* owner)
	: Component(id, owner)
	, m_collider(nullptr)
	, m_colliderWeight(0.0f)
{}

PhysicsComponent::~PhysicsComponent(void)
{
	delete m_collider;
}

Shape& PhysicsComponent::GetColliderShape(void) const
{
	return *m_collider;
}

float PhysicsComponent::GetColliderWeight(void) const
{
	return m_colliderWeight;
}

void PhysicsComponent::SetColliderWeight(float weight)
{
	m_colliderWeight = weight;
}

const std::vector<Vector3>& PhysicsComponent::GetCollisions(void) const
{
	return m_collisions;
}

void PhysicsComponent::AddCollision(const Vector3& normal)
{
	m_collisions.push_back(normal);
}

void PhysicsComponent::ResetCollisions(void)
{
	m_collisions.clear();
}

const Vector3& PhysicsComponent::GetVelocity(void) const
{
	return m_velocity;
}

void PhysicsComponent::SetVelocity(const Vector3& velocity)
{
	m_velocity = velocity;
}
void PhysicsComponent::ResetVelocity(void)
{
	m_velocity = {};
}

const Vector3& PhysicsComponent::GetLastPosition(void) const
{
	return m_collider->m_previousCenter;
}

void PhysicsComponent::Deserialize(const JSONData& source)
{	
	std::string shapeType;
	bool hasTypeData = false;

	if (m_collider != nullptr)
	{ // update collider in case SceneComponent was overriden by World data
		shapeType = m_collider->m_typeStr;
		hasTypeData = true;

		delete m_collider;
		m_collider = nullptr;
	}
	else
	{
		hasTypeData = source.GetString("shape_type", shapeType);
	}

	if (hasTypeData)
	{
		std::vector<SceneComponent*> sceneComps;
		GetOwner()->QueryComponents(sceneComps);
		if (!sceneComps.empty())
		{
			Transform& transform = sceneComps[0]->GetTransform();

			Vector2 halfExtents = { transform.GetScale().x * 0.5f, transform.GetScale().y * 0.5f };

			if (shapeType == "Point")
			{
				m_collider = new Point(transform.GetPosition());
			}
			else if (shapeType == "Circle")
			{
				m_collider = new Circle(transform.GetPosition(), halfExtents.x);
			}
			else if (shapeType == "AABB")
			{
				m_collider = new AABB(transform.GetPosition(), halfExtents);
			}
			else if (shapeType == "OBB")
			{
				m_collider = new OBB(transform.GetPosition(), halfExtents, transform.GetRotation().z);
			}
			else
			{
				fprintf(stderr, "PhysicsComponent::%s: unknown shape type \"%s\"\n", __func__, shapeType.c_str());
			}
		}
		else
		{
			fprintf(stderr, "%s::PhysicsComponent::%s: no valid SceneComponent found\n", GetOwner()->GetObjectTypeName().c_str(), __func__);
		}
	}
	else
	{
		fprintf(stderr, "%s::PhysicsComponent::%s: no valid Shape type found\n", GetOwner()->GetObjectTypeName().c_str(), __func__);
	}

	if (source.HasMember("collider_weight"))
	{
		float weight = 0.0f;
		source.GetFloat("collider_weight", weight);
		m_colliderWeight = weight;
	}
}

Component* PhysicsComponent::Clone(void) const
{
	Component* clone = new PhysicsComponent(*this);
	return clone;
}

const std::string& PhysicsComponent::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& PhysicsComponent::GetClassTypeName(void)
{
	static const std::string name = "PhysicsComponent";
	return name;
}

void PhysicsComponent::NotifyCollision(void)
{
	m_owner->CollisionReact();
}
