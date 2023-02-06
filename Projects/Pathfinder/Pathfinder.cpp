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

#include "Pathfinder.h"
#include "Deploy.h"
#include "App.h"
#include "GameObjectFactory.h"
#include "WorldManager.h"
#include "PFTile.h"
#include "World.h"
#include "SceneComponent.h"
#include "PhysicsComponent.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "Model.h"
#include "Shapes.h"
#include "ColorDefinitions.h"
#include "MessageFileRequest.h"
#include "StringUtility.h"
#include <imgui.h>

constexpr int MAX_PIXELS_ALLOWED = 550; // allows MAX_PIXELS_ALLOWED pixels to draw environment
constexpr int MAX_TILE_SIZE = 50; // ensures that one tile is not larger than MAX_TILE_SIZE pixels
constexpr int DEFAULT_DISPLAY_SPEED_APS = 10; // default speed for displaying navigator actions, APS

namespace MapDebug {
	void PrintStoredMap(const Map2D& map)
	{
#ifdef _DEBUG
		fprintf(stderr, "Stored map data:\n\n");
		for (const MapLine& line : map)
		{
			std::string lineStr;
			for (const Tile& obj : line)
			{
				switch (obj.m_tileState)
				{
				case TileState::Unchecked_Blocked:
					lineStr += '*';
					break;
				case TileState::Unchecked_Empty:
					lineStr += '.';
					break;
				case TileState::Tile_Start:
					lineStr += 'S';
					break;
				case TileState::Tile_Finish:
					lineStr += 'F';
					break;
				case TileState::Checked_Empty:
					lineStr += '+';
					break;
				default:
					break;

				}
			}
			fprintf(stderr, "\t%s\n", lineStr.c_str());
		}
		fprintf(stderr, "\n");
#endif
	}
}

PathfinderSystem::PathfinderSystem(App* app, GameObjectFactory* GOF)
	: GameplaySystem(app, GOF)
	, m_tileScale({ -1.0f, -1.0f, 0.0f })
	, m_agent(nullptr)
	, m_appPtr(nullptr)
{}

PathfinderSystem::~PathfinderSystem(void)
{}

bool PathfinderSystem::Initialize(INIReader* ini)
{
	m_appPtr = App::Get();
	m_UISize = m_appPtr->GetWindowSize();
	int tempSize = static_cast<int>(m_UISize.x * 0.33f); // get size without fractional part
	m_UISize.x = static_cast<float>(tempSize);
	tempSize = static_cast<int>(m_UISize.y * 0.33f);
	m_UISize.y = static_cast<float>(tempSize);

	m_GOF->RegisterObject<PFTile>();

	FileRequestMessage frm;
	frm.m_extension = ".pfmap";
	frm.m_FileLoader = std::bind(&PathfinderSystem::LoadPathfinderMap, this, std::placeholders::_1);
	m_messenger.Send(frm);

	return GameplaySystem::Initialize(ini);
}

namespace UIState
{
	static const struct Defaults
	{
		bool searchAStar = true;
		bool searchBFS = false;
		bool searchDijkstra = false;
	} def;

	// Show windows
	static bool windowPosSizeSet = false;
	static bool showMain = true;
	static bool showError = false;

	// Search algorithm selections
	static bool searchAStar = def.searchAStar;
	static bool searchBFS = def.searchBFS;
	static bool searchDijkstra = def.searchDijkstra;
}

namespace PFState // Runtime states
{
	static bool start = false;
	static bool finish = false;

	static SearchAlgorithm searchType = {};

	static bool mapSelected = false;
	static bool searchSelected = false;
	static std::string selectionName;
	static std::string selectionPath;

	static int speedSetting = DEFAULT_DISPLAY_SPEED_APS;
}

namespace UIText
{
	namespace MainWindow
	{
		static const std::string title = "Pathfinder Menu";
	}

	namespace MapSelector
	{
		static const std::string title = "Select map";
		static std::string selectionDisplay;
	}
	
	namespace SearchSelector
	{
		static const std::string title = "Choose search algorithm:";
		static const std::string AStar = "A*";
		static const std::string BFS = "Greedy Best-First";
		static const std::string Dijkstra = "Dijkstra";
	}

	namespace SpeedSlider
	{
		static const std::string title = "Display actions per second:";
	}

	namespace Errors
	{
		static const std::string noMap = "Select map to start.";
		static const std::string noSearch = "Select search algorithm to start.";
		static const std::string noBoth = "Select map and search algorithm to start.";
	}

	namespace Buttons
	{
		static const std::string start = "START";
		static const std::string restart = "RESTART";
		static const std::string reset = "RESET";
	}
}

void PathfinderSystem::Update(float deltaTime)
{
	if (UIState::showMain)
	{
		ImGui::Begin(UIText::MainWindow::title.c_str());
		if (!UIState::windowPosSizeSet)
		{
			Vector2 size = m_appPtr->GetWindowSize();
			// Set size to 1/3 of the app window size, remove fractional parts.
			size.x = static_cast<float>(static_cast<int>(size.x * 0.33f));
			size.y = static_cast<float>(static_cast<int>(size.y * 0.33f));
			ImGui::SetWindowPos({ 0.0f, 0.0f });
			ImGui::SetWindowSize({ size.x, size.y });
			UIState::windowPosSizeSet = true;
		}

		if (!PFState::finish)
		{
			if (ImGui::BeginMenu(UIText::MapSelector::title.c_str()))
			{
				for (auto fileIter : m_mapFiles)
				{
					if (ImGui::MenuItem(fileIter.first.c_str()))
					{
						PFState::mapSelected = true;
						PFState::selectionName = fileIter.first;
						PFState::selectionPath = fileIter.second;
						UIText::MapSelector::selectionDisplay = "Selected: " + PFState::selectionName;
					}
				}
				ImGui::EndMenu();
			}
		}

		ImGui::Spacing();
		if (PFState::mapSelected)
		{
			ImGui::Text(UIText::MapSelector::selectionDisplay.c_str());
		}
		else
		{
			ImGui::NewLine();
		}
		ImGui::Spacing();

		ImGui::Text(UIText::SearchSelector::title.c_str());
		ImGui::Spacing();

		// Search algorithm selection (selecting one deselects others)
		ImGui::Checkbox(UIText::SearchSelector::AStar.c_str(), &UIState::searchAStar);
		if (UIState::searchAStar) { PFState::searchType = SearchAlgorithm::AStar; UIState::searchBFS = UIState::searchDijkstra = false; }
		ImGui::Checkbox(UIText::SearchSelector::BFS.c_str(), &UIState::searchBFS);
		if (UIState::searchBFS) { PFState::searchType = SearchAlgorithm::BFS; UIState::searchAStar = UIState::searchDijkstra = false; }
		ImGui::Checkbox(UIText::SearchSelector::Dijkstra.c_str(), &UIState::searchDijkstra);
		if (UIState::searchDijkstra) { PFState::searchType = SearchAlgorithm::Dijkstra; UIState::searchAStar = UIState::searchBFS = false; }
		ImGui::Spacing();

		// Checks if all required selections are made
		PFState::searchSelected = UIState::searchAStar || UIState::searchBFS || UIState::searchDijkstra;
		UIState::showError = !(PFState::mapSelected && PFState::searchSelected);

		// Select action display speed
		ImGui::Text(UIText::SpeedSlider::title.c_str());
		ImGui::SliderInt("APS", &PFState::speedSetting, 1, 20);
		ImGui::Spacing();

		// Action buttons
		if (ImGui::Button(PFState::finish ? UIText::Buttons::restart.c_str() : UIText::Buttons::start.c_str()))
		{
			PFState::start = !UIState::showError; // start if no error is displayed
			if (PFState::start)
			{
				if (PFState::finish)
				{
					m_agent = nullptr;
					m_navigator.Clear();
					m_environment.Clear();
					m_worldManager->GetActiveWorld()->Destroy();

					PFState::finish = false;
				}

				m_environment.Initialize(PFState::selectionPath);
				UIState::showMain = false;
			}
		}
		if (PFState::finish)
		{
			if (ImGui::Button(UIText::Buttons::reset.c_str()))
			{
				m_agent = nullptr;
				m_navigator.Clear();
				m_environment.Clear();
				m_worldManager->GetActiveWorld()->Destroy();

				UIState::showMain = true;
				UIState::searchAStar = UIState::def.searchAStar;
				UIState::searchBFS = UIState::def.searchBFS;
				UIState::searchDijkstra = UIState::def.searchDijkstra;
				PFState::start = false;
				PFState::finish = false;
			}
		}

		// Show error if applicable, otherwise empty line
		if (UIState::showError)
		{
			const std::string* errStr = &UIText::Errors::noBoth;
			if (PFState::mapSelected)
			{
				errStr = &UIText::Errors::noSearch;
			}
			if (PFState::searchSelected)
			{
				errStr = &UIText::Errors::noMap;
			}
			ImGui::BeginPopup("Error");
			ImGui::Text(errStr->c_str());
		}
		else
		{
			ImGui::NewLine();
		}

		ImGui::End();
	}

	if (PFState::start)
	{
		static bool pathfinderMapFileLoaded = false;
		static bool pathfinderObjectsCreated = false;
		static bool pathfinderObjectsFinalized = false;

		if (!pathfinderMapFileLoaded)
		{
			std::string worldFile = "PathfinderMapObjects.lvl"; // file containing pathfinder objects (does not change)
			m_worldManager->LoadWorld(WorldType::W_ACTIVE, worldFile);

			pathfinderMapFileLoaded = true;
			return; // skip frame after this
		}
		if (!pathfinderObjectsCreated)
		{
			World* world = m_worldManager->GetActiveWorld();
			PopulateWorld(world);
			m_navigator.Initialize(m_environment);
			MapDebug::PrintStoredMap(m_environment.m_map);

			pathfinderObjectsCreated = true;
			return; // skip frame after this
		}
		if (!pathfinderObjectsFinalized)
		{
			int numLines = m_environment.GetNumLines();
			int numCols = m_environment.GetNumColumns();
			int numTiles = numLines * numCols;

			for (int line = 0; line < numLines; ++line)
			{
				for (int col = 0; col < numCols; ++col)
				{
					const Tile& tile = m_environment.m_map[line][col];
					std::vector<GraphicsComponent*> graphicsComps;
					tile.m_tileObjectPtr->QueryComponents(graphicsComps);
					GraphicsComponent* gc = graphicsComps[0];

					switch (tile.m_tileState)
					{
					case TileState::Unchecked_Blocked:
						gc->m_material->m_color = Color(DARK_GREY);
						break;
					case TileState::Unchecked_Empty:
						gc->m_material->m_color = Color(LIGHT_GREY);
						break;
					case TileState::Tile_Start:
						gc->m_material->m_color = Color(RED);
						break;
					case TileState::Tile_Finish:
						gc->m_material->m_color = Color(BLUE);
						break;
					default:
						break;
					}
				}
			}
			pathfinderObjectsFinalized = true;
		}

		static bool foundRoute = false;
		static bool displayedRoute = false;
		static size_t numTilesInRoute = 0U;
		static size_t currentTileIndex = 0U;
		static bool agentInPosition = true;

		if (foundRoute && displayedRoute)
		{
			if (agentInPosition && currentTileIndex == numTilesInRoute)
			{ // finished, reset everything
				pathfinderMapFileLoaded = false;
				pathfinderObjectsCreated = false;
				pathfinderObjectsFinalized = false;

				foundRoute = displayedRoute = false;
				numTilesInRoute = currentTileIndex = 0U;
				agentInPosition = true;

				UIState::showMain = PFState::finish = true;
				PFState::start = false;
				return;
			}
			if (agentInPosition && currentTileIndex < numTilesInRoute)
			{ // agent in position, determine next action
				/*
					1. Find next action for agent;
					2. Determine next position on map;
					3. Determine next position on screen, set in m_nextAgentPosOnScreen;
					4. Set agentInPosition to false;
				*/

				m_nextAgentPosOnScreen = m_currentAgentPosOnScreen;
				MoveDirection nextMove = m_route[currentTileIndex];
				switch (nextMove)
				{
				case MoveDirection::NORTH:
					m_nextAgentPosOnScreen.y += m_tileScale.y;
					--m_environment.m_agent.m_currentPos.m_line;
					break;
				case MoveDirection::EAST:
					m_nextAgentPosOnScreen.x += m_tileScale.x;
					++m_environment.m_agent.m_currentPos.m_column;
					break;
				case MoveDirection::SOUTH:
					m_nextAgentPosOnScreen.y -= m_tileScale.y;
					++m_environment.m_agent.m_currentPos.m_line;
					break;
				case MoveDirection::WEST:
					m_nextAgentPosOnScreen.x -= m_tileScale.x;
					--m_environment.m_agent.m_currentPos.m_column;
					break;
				default:
					break;
				}

				agentInPosition = false;
				++currentTileIndex;
			}
			else
			{ // agent not in position: animate movement toward destination
				Vector3 remainingDistance = m_nextAgentPosOnScreen - m_currentAgentPosOnScreen;
				Vector3 direction = remainingDistance.Normalize();

				Vector3 agentPosNextFrame = m_currentAgentPosOnScreen + m_agentVelocity * direction * deltaTime;
				Vector3 distanceDestinationToNextFramePos = agentPosNextFrame - m_nextAgentPosOnScreen;
				Vector3 directionDestinationToNextFramePos = distanceDestinationToNextFramePos.Normalize();
				if (directionDestinationToNextFramePos != -direction)
				{ // next frame position equals or goes past destination
					agentPosNextFrame -= distanceDestinationToNextFramePos;
					agentInPosition = true;
				}

				std::vector<SceneComponent*> scene;
				m_agent->QueryComponents(scene);
				scene[0]->GetTransform().SetPosition(agentPosNextFrame);
				m_currentAgentPosOnScreen = agentPosNextFrame;

				std::vector<PhysicsComponent*> physics;
				m_agent->QueryComponents(physics);
				physics[0]->GetColliderShape().m_center = agentPosNextFrame;
			}
		}
		else
		{
			if (!foundRoute)
			{ // 1. calculate route
				foundRoute = m_navigator.FindRoute(PFState::searchType);
				foundRoute = true; // override case where no path is found and it keeps searching
			}
			if (!displayedRoute)
			{ // 2. display A* expansion
				displayedRoute = m_navigator.DisplayNavigationProcess(deltaTime, 1.0f / static_cast<float>(PFState::speedSetting));
			}
			if (displayedRoute)
			{ // 3. get complete route to move agent
				m_route = m_navigator.GetRoute();
				numTilesInRoute = m_route.size();
			}
		}
	}
}

void PathfinderSystem::Exit(void)
{
	m_agent = nullptr;
}

void PathfinderSystem::RegisterMessages(void)
{}

void PathfinderSystem::ProcessMessage(const Message* message)
{}

void PathfinderSystem::RegisterComponents(void) const
{
	GameplaySystem::RegisterComponents();
}

void PathfinderSystem::LoadPathfinderMap(const std::string& path)
{
	std::string filename = StringUtility::ExtractFilenameFromPath(path); // file name with extension
	m_mapFiles.insert({ filename, path });
}

void PathfinderSystem::PopulateWorld(World* world)
{
	if (!m_environment.IsInitialized())
	{
		fprintf(stderr, "PathfinderSystem::%s: environment not initialized.", __func__);
		return;
	}

	// Determine the number of tiles, their size and scale, required to depict the environment
	int numLines = m_environment.GetNumLines();
	int numCols = m_environment.GetNumColumns();
	float numLinesFL = static_cast<float>(numLines);
	float numColsFL = static_cast<float>(numCols);

	int longestSide = numLines > numCols ? numLines : numCols;
	float velocity = static_cast<float>(static_cast<int>(longestSide * 0.5f)); // cover about half of map's longest side, remove fraction part
	int tileScale = MAX_PIXELS_ALLOWED / longestSide;
	if (tileScale > MAX_TILE_SIZE)
	{ 
		tileScale = MAX_TILE_SIZE;
	}
	
	float tileScaleFL = static_cast<float>(tileScale);
	m_tileScale = { tileScaleFL, tileScaleFL, 0.0f };
	velocity *= tileScaleFL;
	m_agentVelocity = { velocity, velocity, velocity }; // set agent velocity to two tiles per second
	float tileHalfExtents = tileScaleFL * 0.5f;
	
	// Offsets for first tile placement
	float xOffset = -(tileHalfExtents * (numColsFL - 1.0f));
	float yOffset = tileHalfExtents * (numLinesFL - 1.0f);
	
	for (int line = 0; line < numLines; ++line)
	{
		for (int col = 0; col < numCols; ++col)
		{
			float tilePosX = xOffset + col * tileScale;
			float tilePosY = yOffset - line * tileScale;
			Vector3 tilePosition = { tilePosX, tilePosY };

			PFTile* tile = m_GOF->CreateNewObject<PFTile>(world);
			std::vector<SceneComponent*> sceneComps;
			tile->QueryComponents(sceneComps);
			Transform& tr = sceneComps[0]->GetTransform();
			tr.SetScale({ tileScaleFL, tileScaleFL, 0.0f });
			tr.SetPosition(tilePosition);

			std::vector<PhysicsComponent*> physComps;
			tile->QueryComponents(physComps);
			AABB& colliderShape = static_cast<AABB&>(physComps[0]->GetColliderShape());
			colliderShape.m_halfExtents = { tileHalfExtents, tileHalfExtents };
			colliderShape.m_center = tilePosition;
			colliderShape.m_previousCenter = tilePosition;

			m_environment.m_map[line][col].m_tileObjectPtr = tile;
		}
	}

	// Add agent object to the map
	float agentPosX = xOffset + static_cast<float>(m_environment.m_agent.m_currentPos.m_column * tileScale);
	float agentPosY = yOffset - static_cast<float>(m_environment.m_agent.m_currentPos.m_line * tileScale);
	m_currentAgentPosOnScreen = { agentPosX, agentPosY };
	m_nextAgentPosOnScreen = m_currentAgentPosOnScreen;

	m_agent = m_GOF->CreateNewObject<PFTile>(world);
	std::vector<SceneComponent*> sceneComps;
	m_agent->QueryComponents(sceneComps);
	Transform& tr = sceneComps[0]->GetTransform();
	tr.SetScale({ tileHalfExtents, tileHalfExtents, 0.0f });
	tr.SetPosition(m_currentAgentPosOnScreen);

	std::vector<PhysicsComponent*> physComps;
	m_agent->QueryComponents(physComps);
	AABB& colliderShape = static_cast<AABB&>(physComps[0]->GetColliderShape());
	colliderShape.m_halfExtents = { tileHalfExtents * 0.5f, tileHalfExtents * 0.5f };
	colliderShape.m_center = m_currentAgentPosOnScreen;
	colliderShape.m_previousCenter = m_currentAgentPosOnScreen;
}

DEPLOY(PathfinderSystem);
