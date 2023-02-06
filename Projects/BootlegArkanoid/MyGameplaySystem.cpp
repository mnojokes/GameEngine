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

#include "MyGameplaySystem.h"
#include "GameObjectFactory.h"
#include "Paddle.h"
#include "Brick.h"
#include "Ball.h"
#include "Wall.h"
#include "App.h"
#include "Deploy.h"
#include "SceneComponent.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "MessagesInput.h"
#include "MessagesGameplay.h"
#include "MessageFileRequest.h"
#include "StringUtility.h"
#include "ColorDefinitions.h"
#include "imgui.h"

namespace GameParams
{
	constexpr int NUM_PLAYS_AVAILABLE = 3;
	constexpr int DEF_BRICK_HIT_SCORE = 100;

	constexpr float START_TIMER_DURATION = 1.5f;
}

namespace UIParams
{
	static Vector2 windowSize = {};
	static Vector2 windowCenter = {};

	static bool windowParamsInitialized = false;
	static bool mainInitialized = false;
	static bool pauseInitialized = false;
	static bool ingameInitialized = false;
	static bool interruptInitialized = false;
	static bool gameOverInitialized = false;

	static bool playTriggered = false;
	static bool showIngameScreen = false;

	static int lastLives = -1;
	static int lastScore = -1;
	static int lastHighScore = -1;

	namespace Text
	{
		namespace Defaults
		{
			static std::string buttonResume = "Resume";
			static std::string buttonQuit = "Quit";
			static std::string buttonRestart = "Restart";
			static std::string buttonContinue = "Continue";
			static std::string buttonRetry = "Retry";
			static std::string buttonTimer;
		}
		namespace InGame
		{
			static std::string title = "Playing";
			static std::string lives = "Lives: ";
			static std::string fullLives;
			static std::string score = "Score: ";
			static std::string fullScore;
			static std::string highScore = "High Score: ";
			static std::string fullHighScore;
		}
		namespace MainMenu
		{
			static std::string title = "Bootleg Arkanoid Main Menu";
			static std::string buttonPlayDefault = "Play (not recommended)";
			static std::string buttonQuitDefault = "Quit (recommended)";
			static std::string buttonQuitWaiting = "Quit (last chance)";
		}
		namespace Pause
		{
			static std::string title = "Pause";
		}
		namespace Interrupt
		{
			static std::string title = "Game Stopped";
			static std::string good = "Level completed!.. About time.";
			static std::string bad = "Wow. That's really embarassing!";
		}
		namespace GameOver
		{
			static std::string title = "Game Over";
			static std::string good = "You won... but at what cost...";
			static std::string bad = "How could you lose?";
		}
	}
}

MyGameplaySystem::MyGameplaySystem(App* app, GameObjectFactory* GOF)
	: GameplaySystem(app, GOF)
	, m_currentMapIndex(0)
	, m_currentScore(0)
	, m_highScore(0)
	, m_livesRemaining(1)
	, m_bricksRemaining(0)
	, m_timer(0.0f)
	, m_gameState(GameState::LoadAssets)
	, m_UIState(UIState::ShowMain)
{}

MyGameplaySystem::~MyGameplaySystem(void)
{}

bool MyGameplaySystem::Initialize(INIReader* ini)
{
	if (m_gameState == GameState::LoadAssets)
	{
		GameObjectFactory* GOF = GameObjectFactory::Get();
		GOF->RegisterObject<Wall>();
		GOF->RegisterObject<Paddle>();
		GOF->RegisterObject<Brick>();
		GOF->RegisterObject<Ball>();

		FileRequestMessage frm;
		frm.m_extension = ".lvl";
		frm.m_FileLoader = std::bind(&MyGameplaySystem::StoreMapFilename, this, std::placeholders::_1);
		m_messenger.Send(frm);
	}

	UIParams::windowSize = m_appPtr->GetWindowSize();
	UIParams::windowCenter = UIParams::windowSize * 0.5f;
	// Remove fractional parts if any
	UIParams::windowCenter.x = static_cast<float>(static_cast<int>(UIParams::windowCenter.x));
	UIParams::windowCenter.y = static_cast<float>(static_cast<int>(UIParams::windowCenter.y));

	m_livesRemaining = GameParams::NUM_PLAYS_AVAILABLE;

	return GameplaySystem::Initialize(ini);
}

void MyGameplaySystem::Update(float deltaTime)
{
	if (m_UIState != UIState::NONE)
	{
		ImVec2 UIWinSize = {};
		float lineHeight = {};

		Vector2 winSize = UIParams::windowCenter * 0.5f;
		Vector2 winPos = UIParams::windowCenter - winSize * 0.5f;

		std::string* quitButtonText = &UIParams::Text::Defaults::buttonQuit;

		if (m_UIState == UIState::ShowMain)
		{
			ImGui::Begin(UIParams::Text::MainMenu::title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			if (!UIParams::mainInitialized)
			{
				ImGui::SetWindowPos({ winPos.x, winPos.y });
				ImGui::SetWindowSize({ winSize.x, winSize.y });
				UIParams::mainInitialized = true;
			}
			else
			{
				// Keep buttons centered
				UIWinSize = ImGui::GetWindowContentRegionMax();
				lineHeight = ImGui::GetTextLineHeight();
				float spacingHeight = 0.0f;
				while (spacingHeight < (UIWinSize.y - ImGui::GetTextLineHeightWithSpacing()) * 0.2f)
				{
					ImGui::NewLine();
					spacingHeight += lineHeight;
				}
				if (!UIParams::playTriggered)
				{
					quitButtonText = &UIParams::Text::MainMenu::buttonQuitDefault;

					if (ImGui::Button(UIParams::Text::MainMenu::buttonPlayDefault.c_str(), { UIWinSize.x, UIWinSize.y * 0.1f }))
					{
						UIParams::playTriggered = true;
					}
				}
				else
				{
					quitButtonText = &UIParams::Text::MainMenu::buttonQuitWaiting;

					m_timer += deltaTime;
					if (m_timer >= GameParams::START_TIMER_DURATION)
					{
						m_timer = 0.0f;
						UIParams::playTriggered = false;
						m_gameState = GetNextGameState(m_gameState);
						m_UIState = UIState::NONE;
					}
					else
					{
						float secondCounter = GameParams::START_TIMER_DURATION - m_timer;
						UIParams::Text::Defaults::buttonTimer = std::to_string(secondCounter);
						ImGui::Button(UIParams::Text::Defaults::buttonTimer.substr(0, 4).c_str(), {UIWinSize.x, UIWinSize.y * 0.1f});
					}
				}
			}
		}
		else if (m_UIState == UIState::ShowPause)
		{
			ImGui::Begin(UIParams::Text::Pause::title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			if (!UIParams::pauseInitialized)
			{
				ImGui::SetWindowPos({ winPos.x, winPos.y });
				ImGui::SetWindowSize({ winSize.x, winSize.y });
				UIParams::pauseInitialized = true;
			}
			else
			{
				// Keep buttons centered
				UIWinSize = ImGui::GetWindowContentRegionMax();
				lineHeight = ImGui::GetTextLineHeight();
				float spacingHeight = 0.0f;
				while (spacingHeight < (UIWinSize.y - ImGui::GetTextLineHeightWithSpacing()) * 0.2f)
				{
					ImGui::NewLine();
					spacingHeight += lineHeight;
				}
				if (ImGui::Button(UIParams::Text::Defaults::buttonResume.c_str(), {UIWinSize.x, UIWinSize.y * 0.1f}))
				{
					m_UIState = UIState::NONE;
					m_appPtr->m_appStateIsRunning = true;
				}
			}
		}
		else if (m_UIState == UIState::ShowInterrupt)
		{
			std::string* interruptText = &UIParams::Text::Interrupt::good;
			if (m_bricksRemaining != 0)
			{
				interruptText = &UIParams::Text::Interrupt::bad;
			}

			ImGui::Begin(UIParams::Text::Interrupt::title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			if (!UIParams::interruptInitialized)
			{
				ImGui::SetWindowPos({ winPos.x, winPos.y });
				ImGui::SetWindowSize({ winSize.x, winSize.y });
				UIParams::interruptInitialized = true;
			}
			else
			{
				ImGui::Text(interruptText->c_str());

				std::string* buttonText = &UIParams::Text::Defaults::buttonRetry;
				if (m_bricksRemaining == 0)
				{
					buttonText = &UIParams::Text::Defaults::buttonContinue;
				}

				// Keep buttons centered
				UIWinSize = ImGui::GetWindowContentRegionMax();
				lineHeight = ImGui::GetTextLineHeight();
				float spacingHeight = 0.0f;
				while (spacingHeight < (UIWinSize.y - ImGui::GetTextLineHeightWithSpacing()) * 0.2f)
				{
					ImGui::NewLine();
					spacingHeight += lineHeight;
				}
				if (!UIParams::playTriggered)
				{
					if (ImGui::Button(buttonText->c_str(), { UIWinSize.x, UIWinSize.y * 0.1f }))
					{
						if (m_bricksRemaining != 0) { --m_currentMapIndex; }
						UIParams::playTriggered = true;
					}
				}
				else
				{
					m_timer += deltaTime;
					if (m_timer >= GameParams::START_TIMER_DURATION)
					{
						m_timer = 0.0f;
						UIParams::playTriggered = false;
						m_worldManager->GetActiveWorld()->Destroy();
						m_gameState = GameState::LoadWorld;
						m_UIState = UIState::NONE;
					}
					else
					{
						float secondCounter = GameParams::START_TIMER_DURATION - m_timer;
						UIParams::Text::Defaults::buttonTimer = std::to_string(secondCounter);
						ImGui::Button(UIParams::Text::Defaults::buttonTimer.substr(0, 4).c_str(), { UIWinSize.x, UIWinSize.y * 0.1f });
					}
				}
			}
		}
		else if (m_UIState == UIState::ShowGameOver)
		{
			std::string* gameOverText = &UIParams::Text::GameOver::good;
			if (m_livesRemaining == 0)
			{
				gameOverText = &UIParams::Text::GameOver::bad;
			}

			ImGui::Begin(UIParams::Text::GameOver::title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			if (!UIParams::gameOverInitialized)
			{
				ImGui::SetWindowPos({ winPos.x, winPos.y });
				ImGui::SetWindowSize({ winSize.x, winSize.y });
				UIParams::gameOverInitialized = true;
			}
			else
			{
				ImGui::Text(gameOverText->c_str());

				// Keep buttons centered
				UIWinSize = ImGui::GetWindowContentRegionMax();
				lineHeight = ImGui::GetTextLineHeight();
				float spacingHeight = 0.0f;
				while (spacingHeight < (UIWinSize.y - ImGui::GetTextLineHeightWithSpacing()) * 0.2f)
				{
					ImGui::NewLine();
					spacingHeight += lineHeight;
				}
				if (!UIParams::playTriggered)
				{
					if (ImGui::Button(UIParams::Text::Defaults::buttonRestart.c_str(), {UIWinSize.x, UIWinSize.y * 0.1f}))
					{
						UIParams::playTriggered = true;
					}
				}
				else
				{
					m_timer += deltaTime;
					if (m_timer >= GameParams::START_TIMER_DURATION)
					{
						m_timer = 0.0f;
						m_currentMapIndex = 0;
						m_livesRemaining = GameParams::NUM_PLAYS_AVAILABLE;
						m_currentScore = 0;
						UIParams::lastLives = -1;
						UIParams::lastScore = -1;
						UIParams::lastHighScore = -1;
						UIParams::playTriggered = false;
						m_worldManager->GetActiveWorld()->Destroy();
						m_gameState = GameState::LoadWorld;
						m_UIState = UIState::NONE;
					}
					else
					{
						float secondCounter = GameParams::START_TIMER_DURATION - m_timer;
						UIParams::Text::Defaults::buttonTimer = std::to_string(secondCounter);
						ImGui::Button(UIParams::Text::Defaults::buttonTimer.substr(0, 4).c_str(), { UIWinSize.x, UIWinSize.y * 0.1f });
					}
				}
			}
		}

		ImGui::Spacing();
		if (ImGui::Button(quitButtonText->c_str(), { UIWinSize.x, UIWinSize.y * 0.1f }))
		{
			m_appPtr->NotifyExit();
		}

		ImGui::End();
	}

	if (m_gameState == GameState::LoadWorld && m_worldManager->IsWorldDataAvailable())
	{
		m_worldManager->LoadWorld(WorldType::W_ACTIVE, m_maps[m_currentMapIndex]);
		++m_currentMapIndex;
		m_gameState = GetNextGameState(m_gameState);
		return; // skip frame
	}

	const std::vector<GameObject*> objects = m_GOF->GetObjectList();

	if (m_gameState == GameState::InitializeAssets)
	{
		m_bricksRemaining = InitializeColors(objects);
		m_gameState = GetNextGameState(m_gameState);
		m_appPtr->m_appStateIsRunning = true;
	}

	if (m_appPtr->m_appStateIsRunning)
	{
		UIParams::showIngameScreen = true;

		for (GameObject* obj : objects)
		{
			if (obj->GetObjectTypeName() == "Brick")
			{
				Brick* br = static_cast<Brick*>(obj);
				if (br->m_remainingLives == 0)
				{
					m_deletable.push_back(obj);
					--m_bricksRemaining;
				}
			}
		}

		for (GameObject* obj : m_deletable)
		{
			m_GOF->DeleteObject(obj);
		}
		m_deletable.clear();

		if (m_bricksRemaining == 0)
		{
			m_appPtr->m_appStateIsRunning = false;
			if (m_currentMapIndex >= m_maps.size())
			{ // all maps finished
				m_UIState = UIState::ShowGameOver;
			}
			else
			{ // load next map
				m_UIState = UIState::ShowInterrupt;
			}
		}
	}

	if (UIParams::showIngameScreen)
	{
		ImGui::Begin(UIParams::Text::InGame::title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		if (UIParams::ingameInitialized)
		{
			if (m_livesRemaining != UIParams::lastLives)
			{
				UIParams::Text::InGame::fullLives = UIParams::Text::InGame::lives + std::to_string(m_livesRemaining);
				UIParams::lastLives = m_livesRemaining;
			}
			if (m_currentScore != UIParams::lastScore)
			{
				UIParams::Text::InGame::fullScore = UIParams::Text::InGame::score + std::to_string(m_currentScore);
				UIParams::lastScore = m_currentScore;
			}
			if (m_highScore != UIParams::lastHighScore)
			{
				UIParams::Text::InGame::fullHighScore = UIParams::Text::InGame::highScore + std::to_string(m_highScore);
				UIParams::lastHighScore = m_highScore;
			}
			
			ImGui::Text(UIParams::Text::InGame::fullLives.c_str());
			ImGui::Text(UIParams::Text::InGame::fullScore.c_str());
			ImGui::Text(UIParams::Text::InGame::fullHighScore.c_str());
		}
		else
		{
			ImGui::SetWindowPos({ 0.0f, 0.0f });
			ImGui::SetWindowSize({ 200.0f, 100.0f });
			UIParams::ingameInitialized = true;
		}
		ImGui::End();
	}

	GameplaySystem::Update(deltaTime);
}

void MyGameplaySystem::Exit(void)
{
	m_worldManager = nullptr;
}

void MyGameplaySystem::RegisterMessages(void)
{
	m_messenger.RegisterMessage<InputMessage>(this);
	m_messenger.RegisterMessage<BrickHitMessage>(this);
	m_messenger.RegisterMessage<LoseConditionMetMessage>(this);
}

void MyGameplaySystem::ProcessMessage(const Message* message)
{
	if (message->IsType<InputMessage>())
	{
		const InputMessage* msg = static_cast<const InputMessage*>(message);
		if (msg->IsKeyTriggered(VIRTUAL_KEYS::K_ESC))
		{
			switch (m_UIState)
			{
			case UIState::ShowMain:
			case UIState::ShowInterrupt:
			case UIState::ShowGameOver:
				m_appPtr->NotifyExit();
				break;
			case UIState::ShowPause:
				m_UIState = UIState::NONE;
				break;
			case UIState::NONE:
				m_UIState = UIState::ShowPause;
				break;
			default:
				break;
			}

			m_appPtr->m_appStateIsRunning = !m_appPtr->m_appStateIsRunning; // pause/resume
		}
	}
	else if (message->IsType<BrickHitMessage>())
	{
		const BrickHitMessage* msg = static_cast<const BrickHitMessage*>(message);
		m_currentScore += GameParams::DEF_BRICK_HIT_SCORE * msg->m_scoreMultiplier;
		if (m_currentScore > m_highScore) { m_highScore = m_currentScore; }
	}
	else if (message->IsType<LoseConditionMetMessage>())
	{
		m_appPtr->m_appStateIsRunning = false;
		--m_livesRemaining;
		if (m_livesRemaining == 0)
		{
			m_UIState = UIState::ShowGameOver;
		}
		else
		{
			m_UIState = UIState::ShowInterrupt;
		}
	}
}

void MyGameplaySystem::RegisterComponents(void) const
{
	GameplaySystem::RegisterComponents();
}

int MyGameplaySystem::InitializeColors(const std::vector<GameObject*>& objects) const
{
	int numBricks = 0;
	for (auto obj : objects)
	{
		const std::string& type = obj->GetObjectTypeName();
		if (type == "Brick")
		{
			++numBricks;
			Brick* br = static_cast<Brick*>(obj);
			br->ChangeColor();
		}
		else if (type == "Wall")
		{
			std::vector<GraphicsComponent*> gc;
			obj->QueryComponents(gc);
			gc[0]->m_material->m_color = Color(COLOR_WALL);
		}
		else if (type == "Paddle")
		{
			std::vector<GraphicsComponent*> gc;
			obj->QueryComponents(gc);
			gc[0]->m_material->m_color = Color(COLOR_PADDLE);
		}
	}

	return numBricks;
}

MyGameplaySystem::GameState MyGameplaySystem::GetNextGameState(const GameState& gs)
{
	int eol = static_cast<int>(GameState::END_LIST);
	int curState = static_cast<int>(gs) + 1;
	if (curState == eol) { curState = 0; } // wrap around to first state
	return static_cast<GameState>(curState);
}

void MyGameplaySystem::StoreMapFilename(const std::string& path)
{
	std::string filename = StringUtility::ExtractFilenameFromPath(path);
	m_maps.push_back(filename);
}

DEPLOY(MyGameplaySystem);
