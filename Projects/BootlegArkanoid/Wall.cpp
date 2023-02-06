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

#include "Wall.h"
#include "GameObjectFactory.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "ColorDefinitions.h"
#include "JSONData.h"
#include "MessagesGameplay.h"
#include "App.h"

void Wall::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<Wall>();
}

Wall::Wall(void)
	: GameObject(0U, nullptr)
	, m_isLosingWall(false)
{}

Wall::Wall(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
	, m_isLosingWall(false)
{}

void Wall::Deserialize(const JSONData& source)
{
	bool isLosing = source.GetBool("losing_wall", isLosing);
	m_isLosingWall = isLosing;
}

GameObject* Wall::Clone(World* parentWorld) const
{
	GameObject* clone = new Wall(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

const std::string& Wall::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& Wall::GetClassTypeName(void)
{
	static const std::string name = "Wall";
	return name;
}

void Wall::CollisionReact(void)
{
	if (m_isLosingWall)
	{
		LoseConditionMetMessage msg;
		App::Get()->GetMessenger().Send(msg);
	}
}
