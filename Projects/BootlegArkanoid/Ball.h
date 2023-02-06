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

#ifndef BALL_H
#define BALL_H

#include "GameObject.h"
#include "Vector3.h"

class Ball : public GameObject
{
public:
	friend class GameObjectFactory;

	static void Register(void);

	void CollisionReact(void) override final {}

private:
	Ball(void);
	Ball(uint64_t id, World* parentWorld);
	Ball(const Ball& rhs) = default;

	void Initialize(void) override;
	void Deserialize(const JSONData& source);

	GameObject* Clone(World* parentWorld) const override;

	void Update(float deltaTime) override;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

private:
	Vector3 m_velocity;
	Vector3 m_direction;
};

#endif