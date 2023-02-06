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

#include "Navigator.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "ColorDefinitions.h"
#include <vector>

// Multiplier used for calculating map tile distance to goal costs (to prevent using floats if fractional numbers occur).
constexpr int TILE_DISTANCE_COST = 10;
constexpr int TILE_BLOCKED_COST = INT32_MAX;

void Navigator::Initialize(Environment& environment)
{
	if (IsInitialized())
	{
		Clear();
	}

	m_environment = &environment;
}

bool Navigator::IsInitialized(void) const
{
	return m_environment != nullptr;
}

bool Navigator::FindRoute(SearchAlgorithm alg)
{
	int G = 0; // traversal cost
	int H = 0; // heuristic cost
	switch (alg)
	{
	case SearchAlgorithm::AStar:
		G = TILE_DISTANCE_COST;
		H = TILE_DISTANCE_COST;
		break;
	case SearchAlgorithm::BFS:
		H = TILE_DISTANCE_COST;
		break;
	default:
		break;
	}

	Node start;
	start.m_coordinates = m_environment->m_agent.m_currentPos;
	start.m_sourceIndex = -1;
	start.m_tilePtr = &m_environment->m_map[start.m_coordinates.m_line][start.m_coordinates.m_column];
	if (start.m_tilePtr->m_tileState == TileState::Tile_Finish)
	{ // stop any further actions if agent starts at goal
		return true;
	}
	m_open.push_back(start);
	PrintNavExpansion(start, 0);

	bool isFoundFinish = false;
	while (!m_open.empty())
	{
		NavAction currentAction;

		int sourceClosedListIndex = static_cast<int>(m_closed.size());
		m_closed.push_back(*m_open.begin());
		m_open.erase(m_open.begin());
		const Node& source = m_closed.back();
		if (source.m_tilePtr->m_tileState == TileState::Unchecked_Empty)
		{
			source.m_tilePtr->m_tileState = TileState::Checked_Empty;
		}

		PrintNavExpansion(source, sourceClosedListIndex, false);
		currentAction.m_expandedTiles.push_back(TileExpansion(source.m_tilePtr, true));
		
		if (source.m_coordinates == m_environment->m_finish)
		{
			isFoundFinish = true;
			m_open.clear();
			continue;
		}

		for (int i = 0; i < 4; ++i)
		{
			Node next;
			next.m_coordinates = GetCoordinatesInDirection(source, static_cast<MoveDirection>(i));
			if (IsValidCoordinates(next.m_coordinates))
			{
				next.m_sourceIndex = sourceClosedListIndex;
				next.m_tilePtr = &m_environment->m_map[next.m_coordinates.m_line][next.m_coordinates.m_column];
				next.m_GCost = source.m_GCost + G;

				bool skipCycle = false;
				switch (next.m_tilePtr->m_tileState)
				{
				case TileState::Unchecked_Blocked:
					next.m_tilePtr->m_tileState = TileState::Checked_Blocked;
					next.m_HCost = TILE_BLOCKED_COST;
					skipCycle = true;
					break;
				case TileState::Checked_Blocked:
				case TileState::Checked_Empty:
				case TileState::Tile_Start:
					skipCycle = true;
					break;
				case TileState::Tile_Finish:
					break;
				default:
					next.m_HCost = GetHeuristicCost(next.m_coordinates, m_environment->m_finish, H);
					break;
				}

				if (skipCycle) { continue; }

				int OLSize = static_cast<int>(m_open.size());
				int OLInsertIndex = OLSize; // open list index for new node insertion sorted by traversal cost
				int OLDuplicateIndex = -1; // open list index for an existing node with higher traversal cost, -1 if no such duplicate found
				bool isFoundCheaperDuplicate = false; // open list already contains a more efficient path to this node
				bool isFoundInsertIndex = false;

				int nextApproximateCost = next.GetApproximateCost();				
				for (int OLIndex = 0; OLIndex < OLSize; ++OLIndex)
				{
					int OLMemberApproximateCost = m_open[OLIndex].GetApproximateCost();
					if (next.m_coordinates == m_open[OLIndex].m_coordinates)
					{
						if (next.m_GCost < m_open[OLIndex].m_GCost)
						{
							OLDuplicateIndex = OLIndex;
						}
						else
						{
							isFoundCheaperDuplicate = true;
							break;
						}
					}
					if (!isFoundInsertIndex && nextApproximateCost < OLMemberApproximateCost)
					{
						OLInsertIndex = OLIndex;
						isFoundInsertIndex = true;
					}
				}
				if (!isFoundCheaperDuplicate)
				{
					if (OLDuplicateIndex >= 0)
					{ // found a more expensive duplicate on open list, remove
						m_open.erase(m_open.begin() + OLDuplicateIndex);
						OLSize = static_cast<int>(m_open.size());
					}
					if (OLInsertIndex < OLSize)
					{
						m_open.insert(m_open.begin() + OLInsertIndex, next);
					}
					else
					{
						m_open.push_back(next);
					}

					PrintNavExpansion(next, OLInsertIndex);
					currentAction.m_expandedTiles.push_back(TileExpansion(next.m_tilePtr, false));
				}
			}
		}
		m_navActions.push_back(currentAction);
	}

	bool result = true;
	if (isFoundFinish)
	{
		int nextCLIndex = static_cast<int>(m_closed.size() - 1U);
		while (nextCLIndex > 0)
		{
			Node& next = m_closed[nextCLIndex];
			int currentCLIndex = next.m_sourceIndex;
			Node& current = m_closed[currentCLIndex];

			MoveDirection dir = GetDirection(current.m_coordinates, next.m_coordinates);
			m_route.insert(m_route.begin(), dir);

			nextCLIndex = next.m_sourceIndex;
		}
	}
	else
	{
		m_open.clear();
		fprintf(stderr, "Navigator::%s: failed to find route to Finish\n", __func__);
		result = false;
	}

	m_closed.clear();
	return result;
}

bool Navigator::DisplayNavigationProcess(float deltaTime, float actionTime)
{
	if (!m_navActions.empty())
	{
		m_currentActionTime += deltaTime;
		if (m_currentActionTime >= actionTime)
		{
			m_currentActionTime = 0.0f;
			if (m_actionIndex < m_navActions.size())
			{
				for (TileExpansion& iter : m_navActions[m_actionIndex].m_expandedTiles)
				{
					switch (iter.m_isClosedList)
					{
					case true:
						ChangeTileColor(iter.m_tilePtr, MEDIUM_GREEN);
						break;
					case false:
						ChangeTileColor(iter.m_tilePtr, BRIGHT_GREEN);
						break;
					}
				}
				++m_actionIndex;
				return false;
			}
			else
			{
				m_actionIndex = 0;
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}

std::vector<MoveDirection> Navigator::GetRoute(void)
{
	return m_route;
}

void Navigator::Clear(void)
{
	m_environment = nullptr;
	m_navActions.clear();
	m_actionIndex = 0U;
	m_route.clear();
	m_currentActionTime = 0.0f;
}

int Navigator::GetHeuristicCost(const MapCoordinates& a, const MapCoordinates& b, int Hvalue) const
{
	int res = 0;
	if (Hvalue != 0)
	{
		int aX = static_cast<int>(a.m_column);
		int aY = static_cast<int>(a.m_line);
		int bX = static_cast<int>(b.m_column);
		int bY = static_cast<int>(b.m_line);

		int absABX = abs(aX - bX);
		int absABY = abs(aY - bY);

		res = (absABX + absABY) * Hvalue;
	}

	return res;
}

MoveDirection Navigator::GetDirection(const MapCoordinates& from, const MapCoordinates& to) const
{
	if (from != to)
	{
		if (to.m_line < from.m_line)
		{
			return MoveDirection::NORTH;
		}
		else if (to.m_line > from.m_line)
		{
			return MoveDirection::SOUTH;
		}
		else
		{
			if (to.m_column < from.m_column)
			{
				return MoveDirection::WEST;
			}
			else if (to.m_column > from.m_column)
			{
				return MoveDirection::EAST;
			}
		}
	}

	return MoveDirection::NODIR;
}

MapCoordinates Navigator::GetCoordinatesInDirection(const Node& origin, MoveDirection dir) const
{
	MapCoordinates ret = origin.m_coordinates;
	switch (dir)
	{
	case MoveDirection::NORTH:
		ret.m_line -= 1U;
		break;
	case MoveDirection::EAST:
		ret.m_column += 1U;
		break;
	case MoveDirection::SOUTH:
		ret.m_line += 1U;
		break;
	case MoveDirection::WEST:
		ret.m_column -= 1U;
		break;
	default:
		break;
	}

	return ret;
}

bool Navigator::IsValidCoordinates(const MapCoordinates& coordinates) const
{
	if (coordinates.m_line >= 0 && coordinates.m_line < m_environment->GetNumLines())
	{
		if (coordinates.m_column >= 0 && coordinates.m_column < m_environment->GetNumColumns())
		{
			return true;
		}
	}

	return false;
}

void Navigator::ChangeTileColor(Tile* tile, const Vector3& color) const
{
	if (tile->m_tileState != TileState::Tile_Start && tile->m_tileState != TileState::Tile_Finish)
	{
		std::vector<GraphicsComponent*> gc;
		tile->m_tileObjectPtr->QueryComponents(gc);
		gc[0]->m_material->m_color = Color(color);
	}
}

void Navigator::PrintNavExpansion(const Node& node, int index, bool openList) const
{
#ifdef _DEBUG
	if (openList)
	{
		fprintf(stderr, "Open list: added (l: %d, c: %d), heuristics (G: %d, H: %d) at ind %d, size %llu\n\n",
			node.m_coordinates.m_line, node.m_coordinates.m_column, node.m_GCost, node.m_HCost, index, m_open.size());
	}
	else
	{
		fprintf(stderr, "OpenList: removed (l: %d, c: %d)\nClosed list: added (l: %d, c: %d), heuristics (G: %d, H: %d), at ind %d, size %llu\n\n",
			node.m_coordinates.m_line, node.m_coordinates.m_column, node.m_coordinates.m_line, node.m_coordinates.m_column,
			node.m_GCost, node.m_HCost, index, m_closed.size());
	}
#endif
}
