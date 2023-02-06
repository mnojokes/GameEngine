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

#include "PFTile.h"
#include "GameObjectFactory.h"
#include "JSONData.h"

void PFTile::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<PFTile>();
}

PFTile::PFTile(void)
	: GameObject(0U, nullptr)
{}

PFTile::PFTile(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
{}

void PFTile::Deserialize(const JSONData& source)
{}

GameObject* PFTile::Clone(World* parentWorld) const
{
	GameObject* clone = new PFTile(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

const std::string& PFTile::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& PFTile::GetClassTypeName(void)
{
	static const std::string name = "PFTile";
	return name;
}
