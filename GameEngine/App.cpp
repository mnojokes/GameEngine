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

#include "App.h"
#include "ThirdParty/INIReader/ini.h"
#include "FileSystem.h"
#include "GameObjectFactory.h"
#include "GameplaySystem.h"
#include "GraphicsSystem.h"
#include "MessagesGraphicsSystem.h"
#include "PhysicsSystem.h"
#include "WorldManager.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

static App* gAppPtr = nullptr;

static const char* winClassName = "Window";
static const std::string Path_AssetFolder = "../../Assets/";

App::App(void)
	: Singleton(this)
	, m_appStateIsRunning(false)
	, m_input(this)
	, m_ini(nullptr)
	, m_running(false)
	, m_hInstance(nullptr)
	, m_GOF(nullptr)
{
	gAppPtr = this;
	m_GOF = new GameObjectFactory(m_messenger);
	m_worldManager = new WorldManager(m_messenger);
}

App::~App(void)
{
	Exit();
	delete m_worldManager;
	delete m_GOF;
	gAppPtr = nullptr;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT App::WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	{
		return true; // ImGui handled input
	}

	if ((WM_KEYFIRST <= uMsg && uMsg <= WM_KEYLAST) || (WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST))
	{
		return gAppPtr->m_input.ProcessInput(uMsg, wParam, lParam);
	}
	else if (uMsg == WM_SIZE)
	{
		uint32_t width = LOWORD(lParam);
		uint32_t height = HIWORD(lParam);

		WindowResizeMessage msg(width, height);
		gAppPtr->GetMessenger().Send(msg);

		return 0;
	}
	else if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	else if (uMsg == WM_CLOSE)
	{
		gAppPtr->NotifyExit();
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool App::InitializeApp(void)
{
	if (m_hInstance != nullptr)
	{
		fprintf(stderr, "App::%s: App instance is already initialized\n", __func__);
		return true;
	}
	
	m_assetPath = Path_AssetFolder + fs::current_path().filename().string() + "/";
	m_ini = new INIReader(m_assetPath + "Settings.ini");
	m_windowSize.x = static_cast<float>(m_ini->GetInteger("App", "WindowSizeX", 800));
	m_windowSize.y = static_cast<float>(m_ini->GetInteger("App", "WindowSizeY", 600));

	std::string windowName = m_ini->Get("App", "WindowName", "Default");
	m_frameCounter.SetFPSLock(m_ini->GetInteger("App", "FPSLock", 0));

	WNDCLASS winClassData = {};
	{
		winClassData.lpszClassName = winClassName;
		m_hInstance = winClassData.hInstance = GetModuleHandle(nullptr);
		winClassData.lpfnWndProc = &WindowProcedure;
		winClassData.style = CS_DBLCLKS;
	}

	ATOM registered = RegisterClass(&winClassData);
	if (!registered)
	{
		DWORD error = GetLastError();
		fprintf(stderr, "App::%s: Failed to register window class. Error %lu\n", __func__, error);
		return false;
	}

	m_window.Create(winClassData, windowName.c_str(), 0, 0, static_cast<int>(m_windowSize.x), static_cast<int>(m_windowSize.y));

	// ImGui setup
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_window.GetHandle());

	/* Create and initialize default systems and app components */
	CreateSystems();
	RegisterMessages();
	RegisterComponents();
	bool initialized = InitializeSystems();
	if (!initialized)
	{
		return false;
	}

	m_window.Show();

	return true;
}

void App::Update(void)
{
	if (m_window.GetHandle() != nullptr)
	{
		m_running = true;
	}
	else
	{
		fprintf(stderr, "App::%s: Window is not initialized\n", __func__);
		return;
	}

	MSG message = {};
	
	while (m_running)
	{
		m_frameCounter.Start();

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		while (PeekMessage(&message, m_window.GetHandle(), 0u, 0u, PM_REMOVE) != FALSE)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		m_input.Finalize();
		m_messenger.Dispatch();
		UpdateSystems(m_frameCounter.GetDelta());
		m_frameCounter.Finish();
	}
}

void App::Exit(void)
{
	if (m_hInstance == nullptr)
	{
		return;
	}

	ExitSystems();

	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	m_input.Reset();
	m_window.Destroy();

	BOOL success = UnregisterClass(winClassName, m_hInstance);
	if (!success)
	{
		DWORD error = GetLastError();
		fprintf(stderr, "App::%s: Failed to unregister window class. Error %lu\n", __func__, error);
		return;
	}

	delete m_ini;
	m_ini = nullptr;
	m_running = false;
	m_hInstance = nullptr;
}

Messenger& App::GetMessenger(void)
{
	return m_messenger;
}

const Window& App::GetWindow(void) const
{
	return m_window;
}

const Vector2& App::GetWindowSize(void) const
{
	return m_windowSize;
}

const std::string& App::GetAssetPath(void) const
{
	return m_assetPath;
}

void App::NotifyExit(void)
{
	m_running = false;
}

void App::CreateSystems(void)
{
	CreateSystem<FileSystem>();
	CreateSystem<PhysicsSystem>();
	CreateSystem<GraphicsSystem>();
}

void App::RegisterMessages(void) const
{
	for (ISystem* system : m_systems)
	{
		system->RegisterMessages();
	}

	m_GOF->RegisterMessages();
	m_worldManager->RegisterMessages();
}

void App::RegisterComponents(void) const
{
	for (ISystem* system : m_systems)
	{
		system->RegisterComponents();
	}
}

bool App::InitializeSystems(void)
{
	for (ISystem* system : m_systems)
	{
		bool result = system->Initialize(m_ini);
		if (!result)
		{
			return false;
		}
	}
	return true;
}

void App::UpdateSystems(float deltaTime)
{
	for (ISystem* system : m_systems)
	{
		system->Update(deltaTime);
	}
}

void App::ExitSystems(void)
{
	for (ISystem* system : m_systems)
	{
		system->Exit();
		delete system;
	}

	m_systems.clear();
}
