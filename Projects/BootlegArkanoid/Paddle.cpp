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

#include "Paddle.h"
#include "GameObjectFactory.h"
#include "PhysicsComponent.h"
#include "SceneComponent.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "MessagesInput.h"
#include "Shapes.h"
#include "World.h"
#include "JSONData.h"

#include <functional>

void Paddle::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<Paddle>();
}

Paddle::Paddle(void)
	: GameObject(0U, nullptr)
	, m_lastAction(GameActions::None)
	, m_actionBlocked(GameActions::None)
{}

Paddle::Paddle(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
	, m_lastAction(GameActions::None)
	, m_actionBlocked(GameActions::None)
{}

void Paddle::Initialize(void)
{
	std::vector<InputComponent*> inputComp;
	QueryComponents(inputComp);

#ifdef _DEBUG
	if (inputComp.size() != 1)
	{
		fprintf(stderr, "Paddle::%s: found %llu InputComponents, should have 1\n", __func__, inputComp.size());
	}
#endif

	if (!inputComp.empty())
	{
		std::function<void(const InputMessage*)> fn = std::bind(&Paddle::ProcessInput, this, std::placeholders::_1);
		inputComp[0]->RegisterCallback(&fn);
	}
}

void Paddle::Deserialize(const JSONData& source)
{
	source.GetVector3("velocity", m_defaultVelocity);
}

GameObject* Paddle::Clone(World* parentWorld) const
{
	GameObject* clone = new Paddle(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

void Paddle::Update(float deltaTime)
{
	std::vector<PhysicsComponent*> physComp;
	QueryComponents(physComp);
	physComp[0]->ResetVelocity();

	size_t numActions = m_actionQueue.size();
	if (numActions > 0U)
	{
		Vector3 move = {};
		for (size_t i = 0; i < m_actionQueue.size(); ++i)
		{
			GameActions action = m_actionQueue[i];
			float axisVelocity = 0.0f;
			if (action != m_actionBlocked)
			{
				switch (action)
				{
				case GameActions::MoveLeft:
					axisVelocity = deltaTime * -m_defaultVelocity.x;
					if (move.x != axisVelocity) { move.x += axisVelocity; }
					break;
				case GameActions::MoveRight:
					axisVelocity = deltaTime * m_defaultVelocity.x;
					if (move.x != axisVelocity) { move.x += axisVelocity; }
					break;
#ifdef _DEBUG
				case GameActions::MoveUp:
					axisVelocity = deltaTime * m_defaultVelocity.y;
					if (move.y != axisVelocity) { move.y += axisVelocity; }
					break;
				case GameActions::MoveDown:
					axisVelocity = deltaTime * -m_defaultVelocity.y;
					if (move.y != axisVelocity) { move.y += axisVelocity; }
					break;
#endif
				default:
					break;
				}
			}
		}

		physComp[0]->SetVelocity(move);
		m_actionQueue.clear();
	}
}

void Paddle::ProcessInput(const InputMessage* msg)
{
	if (msg->IsKeyTriggered(VIRTUAL_KEYS::K_LEFT) || msg->IsKeyPressed(VIRTUAL_KEYS::K_LEFT))
	{
		m_actionQueue.push_back(GameActions::MoveLeft);
	}
	else if (msg->IsKeyTriggered(VIRTUAL_KEYS::K_RIGHT) || msg->IsKeyPressed(VIRTUAL_KEYS::K_RIGHT))
	{
		m_actionQueue.push_back(GameActions::MoveRight);
	}
#ifdef _DEBUG
	else if (msg->IsKeyTriggered(VIRTUAL_KEYS::K_UP) || msg->IsKeyPressed(VIRTUAL_KEYS::K_UP))
	{
		m_actionQueue.push_back(GameActions::MoveUp);
	}
	else if (msg->IsKeyTriggered(VIRTUAL_KEYS::K_DOWN) || msg->IsKeyPressed(VIRTUAL_KEYS::K_DOWN))
	{
		m_actionQueue.push_back(GameActions::MoveDown);
	}
#endif
}

const std::string& Paddle::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& Paddle::GetClassTypeName(void)
{
	static const std::string name = "Paddle";
	return name;
}
