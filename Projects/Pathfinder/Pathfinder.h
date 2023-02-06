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

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "GameplaySystem.h"
#include "Navigator.h"
#include "Vector2.h"

class PathfinderSystem : public GameplaySystem
{
public:
	PathfinderSystem(App* app, GameObjectFactory* GOF);
	~PathfinderSystem(void);

	bool Initialize(INIReader* ini) override final;
	void Update(float deltaTime) override final;
	void Exit(void) override final;

	void RegisterMessages(void) override final;
	void ProcessMessage(const Message* message) override final;

	void RegisterComponents(void) const override final;

	void LoadPathfinderMap(const std::string& path);

private:
	void PopulateWorld(World* world);

private:
	std::map<std::string, std::string> m_mapFiles; // stores paths to map files found in Assets
	Environment m_environment;
	Navigator m_navigator;
	
	Vector3 m_tileScale;

	GameObject* m_agent;
	Vector3 m_agentVelocity;
	Vector3 m_currentAgentPosOnScreen;
	Vector3 m_nextAgentPosOnScreen;

	std::vector<MoveDirection> m_route;

	const App* m_appPtr; // App pointer to access window parameters
	Vector2 m_UISize;
};

#endif
