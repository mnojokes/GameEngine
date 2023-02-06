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

#ifndef APP_H
#define APP_H

#include "ISystem.h"
#include "Singleton.h"
#include "Window.h"
#include "Input.h"
#include "Messenger.h"
#include "FrameCounter.h"
#include "Vector2.h"
#include <vector>

class GameObjectFactory;
class WorldManager;

class App : public Singleton<App>
{
public:
	App(void);
	~App(void);

	static LRESULT WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	template<typename ClientGameplaySystemType>
	bool Initialize(void);
	void Update(void);
	void Exit(void);

	Messenger& GetMessenger(void);
	const Window& GetWindow(void) const;
	const Vector2& GetWindowSize(void) const;
	const std::string& GetAssetPath(void) const;

	void NotifyExit(void);

public:
	bool m_appStateIsRunning; // controlling variable for pause/resume. False for pause.

private:
	bool InitializeApp(void);
	void CreateSystems(void);
	void RegisterMessages(void) const;	// register messages for systems
	void RegisterComponents(void) const; // register components for systems
	bool InitializeSystems(void);
	void UpdateSystems(float deltaTime);
	void ExitSystems(void);

	template <typename SystemType>
	void CreateSystem(void);	// creates a system of SystemType

private:
	std::vector<ISystem*> m_systems;
	INIReader* m_ini;

	Messenger m_messenger;
	Window m_window;
	Input m_input;
	std::string m_assetPath; // stores path to assets

	GameObjectFactory* m_GOF;
	WorldManager* m_worldManager;

	bool m_running;

	Vector2 m_windowSize;

	/* Window class data */
	HINSTANCE m_hInstance;

	/* Frame time counter */
	FrameCounter m_frameCounter;
};

template<typename ClientGameplaySystemType>
bool App::Initialize(void)
{
	CreateSystem<ClientGameplaySystemType>();
	return InitializeApp();
}

template <typename SystemType>
void App::CreateSystem(void)
{
	ISystem* system = new SystemType(this, m_GOF);
	m_systems.push_back(system);
}

#endif
