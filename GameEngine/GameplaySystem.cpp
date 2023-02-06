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

#include "GameplaySystem.h"
#include "GameObjectFactory.h"
#include "InputComponent.h"
#include "Camera.h"
#include "App.h"

GameplaySystem::GameplaySystem(App* app, GameObjectFactory* GOF)
	: ISystem(app, GOF)
	, m_worldManager(nullptr)
{}

GameplaySystem::~GameplaySystem(void)
{}

bool GameplaySystem::Initialize(INIReader* ini)
{
	m_worldManager = WorldManager::Get();
	return true;
}

void GameplaySystem::Update(float deltaTime)
{
	m_worldManager->UpdateWorlds(deltaTime);
}

void GameplaySystem::Exit(void)
{}
	 
void GameplaySystem::RegisterMessages(void)
{}

void GameplaySystem::ProcessMessage(const Message* message)
{}

void GameplaySystem::RegisterComponents(void) const
{
	InputComponent::Register();
	Camera::Register();
}
