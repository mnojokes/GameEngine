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

#include "PhysicsSystem.h"
#include "GameObjectFactory.h"
#include "MathConstants.h"
#include "App.h"
#include "Vector4.h"
#include "PhysicsComponent.h"
#include "SceneComponent.h"
#include "Collision.h"
#include <math.h>

namespace PhysicsDebug
{
	void PrintCollisionData(const PhysicsComponent* physComp, const Shape* colliderShape, const Collision::ShapeCollision* collision)
	{
#ifdef _DEBUG
		if (physComp != nullptr && colliderShape != nullptr && collision != nullptr)
		{
			std::string shapeName = physComp->GetOwner()->GetObjectTypeName();
			std::string shapeType = colliderShape->m_typeStr;

			fprintf(stderr, "Collision: %s (%s): time: %f, normal: %f, %f, %f\n", shapeName.c_str(), shapeType.c_str(),
				collision->m_time, collision->m_normal.x, collision->m_normal.y, collision->m_normal.z);
		}
#endif
	}
};

PhysicsSystem::PhysicsSystem(App* app, GameObjectFactory* GOF)
	: ISystem(app, GOF)
{}

PhysicsSystem::~PhysicsSystem(void)
{}

bool PhysicsSystem::Initialize(INIReader* ini)
{
	// TO DO

	return true;
}

void PhysicsSystem::Update(float deltaTime)
{
	if (m_appPtr->m_appStateIsRunning)
	{
		m_GOF->GetComponentsOfType(m_physicsComponents);
	}
	else
	{
		m_physicsComponents.clear();
	}

	if (!m_physicsComponents.empty())
	{
		size_t numComps = m_physicsComponents.size();

		for (size_t i = 0U; i < numComps; ++i)
		{
			PhysicsComponent* physicsComponent = m_physicsComponents[i];

			Shape& colliderShape = physicsComponent->GetColliderShape();
			colliderShape.m_previousCenter = colliderShape.m_center;
			colliderShape.m_center += physicsComponent->GetVelocity();
		}

		for (size_t i = 0U; i < numComps - 1U; ++i)
		{
			PhysicsComponent* aPhysComp = m_physicsComponents[i];
			Shape& aColliderShape = aPhysComp->GetColliderShape();

			for (size_t j = i + 1U; j < numComps; ++j)
			{
				PhysicsComponent* bPhysComp = m_physicsComponents[j];
				Shape& bColliderShape = bPhysComp->GetColliderShape();

				if (aPhysComp->GetColliderWeight() != 0.0f && bPhysComp->GetColliderWeight() != 0.0f)
				{ // skip collision processing if any of the objects has 0.0 collider weight
					Collision::CollisionResult collision = Collision::IsCollision(aColliderShape, bColliderShape);

					if (collision.A.isCollision || collision.B.isCollision)
					{
						bool isAMoved = aColliderShape.m_center != aColliderShape.m_previousCenter;
						bool isBMoved = bColliderShape.m_center != bColliderShape.m_previousCenter;

						PhysicsComponent* adjustablePhysComp = nullptr;
						Shape* adjustableShape = nullptr;
						Collision::CollisionEvent* activeCollisionEvent = nullptr;

						if (isAMoved && isBMoved)
						{
							// adjust lighter object relative to heavier object
							// in case objects are equal, adjust second object relative to first
							if (aPhysComp->GetColliderWeight() < bPhysComp->GetColliderWeight())
							{
								adjustablePhysComp = aPhysComp;
								adjustableShape = &aColliderShape;
								activeCollisionEvent = &collision.A;
							}
							else
							{
								adjustablePhysComp = bPhysComp;
								adjustableShape = &bColliderShape;
								activeCollisionEvent = &collision.B;
							}
						}
						else if (isAMoved)
						{
							adjustablePhysComp = aPhysComp;
							adjustableShape = &aColliderShape;
							activeCollisionEvent = &collision.A;
						}
						else if (isBMoved)
						{
							adjustablePhysComp = bPhysComp;
							adjustableShape = &bColliderShape;
							activeCollisionEvent = &collision.B;
						}

						if (adjustablePhysComp != nullptr && adjustableShape != nullptr && activeCollisionEvent != nullptr)
						{
							PhysicsDebug::PrintCollisionData(adjustablePhysComp, adjustableShape, &activeCollisionEvent->thisShape);

							adjustableShape->m_center = adjustableShape->m_previousCenter + adjustablePhysComp->GetVelocity() * activeCollisionEvent->thisShape.m_time;
							adjustablePhysComp->AddCollision(activeCollisionEvent->collidingShape.m_normal);
						}
						else
						{
							std::string failed;
							if (adjustablePhysComp == nullptr) { failed += "adjustablePhysComp, "; }
							if (adjustableShape == nullptr) { failed += "adjustableShape, "; }
							if (activeCollisionEvent == nullptr) { failed += "activeCollisionEvent"; }
							fprintf(stderr, "PhysicsSystem::%s: could not resolve collision. %s == nullptr\n", __func__, failed.c_str());
							return;
						}

						aPhysComp->NotifyCollision();
						bPhysComp->NotifyCollision();
					}
				}

				std::vector<SceneComponent*> sceneComp;
				bPhysComp->GetOwner()->QueryComponents<SceneComponent>(sceneComp);
				Transform& t = sceneComp[0]->GetTransform();
				t.SetPosition(bColliderShape.m_center);
			}

			std::vector<SceneComponent*> sceneComp;
			aPhysComp->GetOwner()->QueryComponents(sceneComp);
			Transform& t = sceneComp[0]->GetTransform();
			t.SetPosition(aColliderShape.m_center);
		}
	}

	m_GOF->GetComponentsOfType(m_sceneComponents);
	for (SceneComponent* sceneComponent : m_sceneComponents)
	{
		Matrix transformMatrix = AssembleNewMatrix(sceneComponent, deltaTime);
		sceneComponent->SetTransform(transformMatrix);
	}
}

void PhysicsSystem::Exit(void)
{
	// TO DO
}

void PhysicsSystem::RegisterMessages(void)
{
	// TO DO
}

void PhysicsSystem::ProcessMessage(const Message* message)
{
	// TO DO
}

void PhysicsSystem::RegisterComponents(void) const
{
	PhysicsComponent::Register();
	SceneComponent::Register();
}

Matrix PhysicsSystem::AssembleNewMatrix(const SceneComponent* sourceScene, float deltaTime,
	bool omitParentScale, bool omitParentRotation, bool omitParentPosition) const
{
	const GameObject* source = sourceScene->GetOwner();

	const Transform& sourceTransform = sourceScene->GetTransform();
	Matrix m = Matrix::Translate(sourceTransform.GetPosition());
	m *= Matrix::Rotate(sourceTransform.GetRotation());
	m *= Matrix::Scale(sourceTransform.GetScale());

	if (source->m_parent != nullptr)
	{
		std::vector<Component*> compQuery;
		source->m_parent->QueryComponents("SceneComponent", compQuery);
		if (!compQuery.empty())
		{
			const SceneComponent* parentSceneComp = static_cast<SceneComponent*>(compQuery[0]);
			const Transform& parentTransform = parentSceneComp->GetTransform();
			Matrix transform = Matrix::Identity();

			if (!omitParentPosition)
			{
				transform *= Matrix::Translate(parentTransform.GetPosition());
			}
			if (!omitParentRotation)
			{
				const Vector3 parentRotation = (parentTransform.GetRotation() * rcp180) * PI;
				transform *= Matrix::Rotate(parentRotation);
			}
			if (!omitParentScale)
			{
				transform *= Matrix::Scale(parentTransform.GetScale());
			}

			m *= transform;
		}
	}

	return m;
}
