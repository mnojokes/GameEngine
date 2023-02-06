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

#ifndef GAMEPLAYSYSTEM_H
#define GAMEPLAYSYSTEM_H

#include "ISystem.h"
#include "WorldManager.h"

class GameObjectFactory;

class GameplaySystem : public ISystem
{
public:
	GameplaySystem(App* app, GameObjectFactory* GOF);
	~GameplaySystem(void);

	bool Initialize(INIReader* ini) override;
	void Update(float deltaTime) override;
	void Exit(void) override;

	void RegisterMessages(void) override;
	void ProcessMessage(const Message* message) override;

	void RegisterComponents(void) const override;

protected:
	WorldManager* m_worldManager;
};

#endif
