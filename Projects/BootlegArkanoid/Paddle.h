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

#ifndef PADDLE_H
#define PADDLE_H

#include "GameObject.h"
#include "Vector3.h"

class InputMessage;

enum class GameActions
{
	None,
	MoveLeft,
	MoveRight
#ifdef _DEBUG
	, MoveUp
	, MoveDown
#endif
};

class Paddle : public GameObject
{
public:
	friend class GameObjectFactory;

	static void Register(void);

	void CollisionReact(void) override final {}

private:
	Paddle(void);
	Paddle(uint64_t id, World* parentWorld);
	Paddle(const Paddle& rhs) = default;

	void Initialize(void) override;
	void Deserialize(const JSONData& source);

	GameObject* Clone(World* parentWorld) const override;

	void Update(float deltaTime) override;
	void ProcessInput(const InputMessage* msg);

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

private:
	std::vector<GameActions> m_actionQueue; // queue of actions to be performed by object this frame
	GameActions m_lastAction; // last performed action
	GameActions m_actionBlocked;

	Vector3 m_defaultVelocity; // workaround - game needs only basic physical properties
};

#endif
