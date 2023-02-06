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

#include "Brick.h"
#include "GameObjectFactory.h"
#include "JSONData.h"
#include "ColorDefinitions.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "MessagesGameplay.h"
#include "Messenger.h"
#include "App.h"

void Brick::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<Brick>();
}

Brick::Brick(void)
	: GameObject(0U, nullptr)
	, m_startingLives(1)
	, m_remainingLives(1)
{}

Brick::Brick(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
	, m_startingLives(1)
	, m_remainingLives(1)
{}

void Brick::Deserialize(const JSONData& source)
{
	int lives = -1;
	bool hasValue = source.GetInt32("lives", lives);
	if (hasValue)
	{
		m_startingLives = m_remainingLives = lives;
	}
}

GameObject* Brick::Clone(World* parentWorld) const
{
	GameObject* clone = new Brick(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

const std::string& Brick::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& Brick::GetClassTypeName(void)
{
	static const std::string name = "Brick";
	return name;
}

void Brick::CollisionReact(void)
{
	if (m_remainingLives > 0)
	{
		--m_remainingLives;
		if (m_remainingLives > 0)
		{
			ChangeColor();
		}
		BrickHitMessage msg;
		msg.m_scoreMultiplier = m_startingLives - m_remainingLives; // every hit increases score multiplier

		App::Get()->GetMessenger().Send(msg);
	}
}

void Brick::ChangeColor(void)
{
	std::vector<GraphicsComponent*> gc;
	QueryComponents(gc);

	switch (m_remainingLives)
	{
	case 3:
		gc[0]->m_material->m_color = Color(COLOR_BRICK_3);
		break;
	case 2:
		gc[0]->m_material->m_color = Color(COLOR_BRICK_2);
		break;
	case 1:
		gc[0]->m_material->m_color = Color(COLOR_BRICK_1);
		break;
	default:
		gc[0]->m_material->m_color = Color({ 1.0f, 1.0f, 1.0f });
		break;
	}
}
