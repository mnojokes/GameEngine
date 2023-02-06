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

#ifndef WALL_H
#define WALL_H

#include "GameObject.h"
#include "Vector3.h"

class InputMessage;

class Wall : public GameObject
{
public:
	friend class GameObjectFactory;

	static void Register(void);

	void CollisionReact(void) override final;

private:
	Wall(void);
	Wall(uint64_t id, World* parentWorld);
	Wall(const Wall& rhs) = default;

	void Initialize(void) override {}
	void Deserialize(const JSONData& source);

	GameObject* Clone(World* parentWorld) const override;

	void Update(float deltaTime) override {}
	void ProcessInput(const InputMessage* msg) {}

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

private:
	bool m_isLosingWall; // if true, game ends when this wall is touched
};


#endif