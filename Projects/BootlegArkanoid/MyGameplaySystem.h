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

#ifndef MYGAMEPLAYSYSTEM_H
#define MYGAMEPLAYSYSTEM_H

#include "GameplaySystem.h"

class MyGameplaySystem : public GameplaySystem
{
public:
	MyGameplaySystem(App* app, GameObjectFactory* GOF);
	~MyGameplaySystem(void);

	bool Initialize(INIReader* ini) override final;
	void Update(float deltaTime) override final;
	void Exit(void) override final;

	void RegisterMessages(void) override final;
	void ProcessMessage(const Message* message) override final;

	void RegisterComponents(void) const override final;

private:
	enum class GameState;

	int InitializeColors(const std::vector<GameObject*>& objects) const; // sets correct colors to all objects returns number of bricks (required for win condition)
	GameState GetNextGameState(const GameState& gs);

	void StoreMapFilename(const std::string& path);

private:
	std::vector<std::string> m_maps;
	int m_currentMapIndex;

	std::vector<GameObject*> m_deletable;	// storage for deletable objects

	int m_currentScore;
	int m_highScore;

	int m_livesRemaining; // number of plays remaining before game over
	int m_bricksRemaining; // number of bricks remaining before level completed

	float m_timer;

	enum class GameState
	{
		LoadAssets,
		LoadWorld,
		InitializeAssets,
		END_LIST
	} m_gameState;

	enum class UIState
	{
		NONE,
		ShowMain,
		ShowPause,
		ShowInterrupt, // game interrupted by level change or intermediate loss
		ShowGameOver // game over after losing all lives or finishing all levels
	} m_UIState;
};

#endif