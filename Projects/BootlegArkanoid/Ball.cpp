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

#include "Ball.h"
#include "GameObjectFactory.h"
#include "JSONData.h"
#include "PhysicsComponent.h"

void Ball::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<Ball>();
}

Ball::Ball(void)
	: GameObject(0U, nullptr)
{}

Ball::Ball(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
{}

void Ball::Initialize(void)
{
	m_direction = m_direction.Normalize();
}

void Ball::Deserialize(const JSONData& source)
{
	source.GetVector3("velocity", m_velocity);
	source.GetVector3("direction", m_direction);
}

GameObject* Ball::Clone(World* parentWorld) const
{
	GameObject* clone = new Ball(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

void Ball::Update(float deltaTime)
{
	std::vector<PhysicsComponent*> physComp;
	QueryComponents(physComp);

	Vector3 newDirection = m_direction;

	const std::vector<Vector3>& collisions = physComp[0]->GetCollisions();
	for (Vector3 col : collisions)
	{
		newDirection = newDirection - (2 * col) * (col.Dot(newDirection));
	}

	m_direction = newDirection.Normalize();
	Vector3 newVelocity = m_velocity * m_direction;
	
	physComp[0]->SetVelocity(newVelocity * deltaTime);
	physComp[0]->ResetCollisions();
}

const std::string& Ball::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& Ball::GetClassTypeName(void)
{
	static const std::string name = "Ball";
	return name;
}
