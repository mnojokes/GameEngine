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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>
#include <string>
#include "Vector3.h"

class GameObject;
struct Tile;

// Vector of Tile objects
typedef std::vector<Tile> MapLine;
// Vector containing MapLine objects
typedef std::vector<MapLine> Map2D;

// MapCoordinates - stores indices for object in Map2D container
struct MapCoordinates
{
    MapCoordinates(void) : m_line(0), m_column(0) {}
    MapCoordinates(int line, int column) : m_line(line), m_column(column) {}
    bool operator==(const MapCoordinates& rhs) const { return m_line == rhs.m_line && m_column == rhs.m_column; }
    bool operator!=(const MapCoordinates& rhs) const { return !(*this == rhs); }

    int m_line;
    int m_column;
};

enum class TileState
{
    Tile_Start,
    Tile_Finish,
    Unchecked_Blocked,
    Unchecked_Empty,
    Checked_Blocked,
    Checked_Empty
};

struct Tile
{
    Tile(void) : m_tileObjectPtr(nullptr), m_tileState(TileState::Unchecked_Empty) {}

    GameObject* m_tileObjectPtr;
    TileState m_tileState;
};

struct Agent
{
    Agent(void) : m_agentObjectPtr(nullptr) {}

    GameObject* m_agentObjectPtr;
    MapCoordinates m_currentPos;
};

class Environment
{
public:
    Environment(void);
    ~Environment(void);
    
    bool Initialize(const std::string& filename);
    bool IsInitialized(void) const;

    int GetNumColumns(void) const;
    int GetNumLines(void) const;
    int GetNumTiles(void) const;

    void Clear(void);

private:
    bool StoreLineOnMap(const std::string& line);

public:
    // map contains tile data - current tile states and GameObject* for corresponding tile, use similar to 2D vector
    Map2D m_map;
    Agent m_agent;

    MapCoordinates m_start;
    MapCoordinates m_finish;

private:
    bool m_isInitialized;
    bool m_hasStart;
    bool m_hasFinish;
};

#endif
