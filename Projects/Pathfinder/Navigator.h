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

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Environment.h"

enum class SearchAlgorithm
{
	AStar,
	BFS,
	Dijkstra
};

enum class NavigatorState
{
	Routing,
	Finished
};

enum class MoveDirection
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NODIR
};

class Navigator
{
public:
	Navigator(void) : m_environment(nullptr), m_currentActionTime(0.0f), m_actionIndex(0) {}

	void Initialize(Environment& environment);
	bool IsInitialized(void) const;

	// FindRoute calculates the path agent needs to take to reach Finish
	bool FindRoute(SearchAlgorithm alg = SearchAlgorithm::Dijkstra);
	
	// DisplayNavigationProcess displays tile expansion, returns true when finished, otherwise returns false
	bool DisplayNavigationProcess(float deltaTime, float actionTime);
	std::vector<MoveDirection> GetRoute(void);

	void Clear(void);

private:
	// Node stores the necessary tile and heuristic data for pathfinding
	struct Node
	{
		Node(void) : m_coordinates({}), m_tilePtr(nullptr), m_sourceIndex(0), m_HCost(0), m_GCost(0) {}

		// GetApproximateCost returns the total F cost of the node
		int GetApproximateCost(void) const { return m_HCost + m_GCost; }

		MapCoordinates m_coordinates;
		Tile* m_tilePtr;
		int m_sourceIndex;
		int m_HCost; // distance to goal cost
		int m_GCost; // agent action cost
	};

	// TileExpansion stores tile pointer and bool value which is true if tile is on closed list, false if on open.
	struct TileExpansion
	{
		TileExpansion(Tile* tilePtr, bool isClosedList) : m_tilePtr(tilePtr), m_isClosedList(isClosedList) {}

		Tile* m_tilePtr;
		bool m_isClosedList;
	};

	// NavAction stores all tiles that were expanded per single navigator action
	struct NavAction
	{
		std::vector<TileExpansion> m_expandedTiles;
	};

	// Calculates absolute distance between two points using Manhattan style
	int GetHeuristicCost(const MapCoordinates& a, const MapCoordinates& b, int Hvalue) const;

	MoveDirection GetDirection(const MapCoordinates& from, const MapCoordinates& to) const;
	MapCoordinates GetCoordinatesInDirection(const Node& origin, MoveDirection dir) const;
	bool IsValidCoordinates(const MapCoordinates& coordinates) const;

	void ChangeTileColor(Tile* tile, const Vector3& color) const;
	void PrintNavExpansion(const Node& node, int index, bool openList = true) const;

private:
	Environment* m_environment; // pointer to current environment

	// A* pathfinding lists */
	std::vector<Node> m_open; // open list
	std::vector<Node> m_closed; // closed list

	// Stores actions of path calculation for visual display
	std::vector<NavAction> m_navActions;
	size_t m_actionIndex; // index of action to be displayed this turn

	// Stores actions required for agent to reach Finish
	std::vector<MoveDirection> m_route;

	// Timing - used to time actions such as how fast tile color are changed during visual routing
	float m_currentActionTime;
};

#endif