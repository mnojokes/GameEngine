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

#include "Environment.h"
#include <fstream>

constexpr int MIN_LINE_SIZE = 3;

Environment::Environment(void)
	: m_isInitialized(false)
	, m_hasStart(false)
	, m_hasFinish(false)
{}

Environment::~Environment(void)
{}

bool Environment::Initialize(const std::string& filename)
{
	if (m_isInitialized)
	{
		Clear();
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		fprintf(stderr, "Environment::%s: cannot open \"%s\"\n", __func__, filename.c_str());
		return false;
	}

	bool success = true;
	std::string line;
	while (std::getline(file, line))
	{
		success = StoreLineOnMap(line);
		if (!success) { break; }
	}

	if (success)
	{
		if (!m_hasStart || !m_hasFinish)
		{
			fprintf(stderr, "Environment::%s: map does not have correct Start and/or Finish location\n", __func__);
			success = false;
		}
		int numLines = GetNumLines();
		if (numLines < MIN_LINE_SIZE)
		{
			fprintf(stderr, "Environment::%s: map should have at least %d lines\n", __func__, numLines);
			success = false;
		}
	}

	m_isInitialized = success;
	return success;
}

bool Environment::IsInitialized(void) const
{
	return m_isInitialized;
}

int Environment::GetNumColumns(void) const
{
	int ret = 0;
	if (!m_map.empty())
	{
		ret = static_cast<int>(m_map[0].size());
	}
	return ret;
}

int Environment::GetNumLines(void) const
{
	return static_cast<int>(m_map.size());
}

int Environment::GetNumTiles(void) const
{
	return GetNumColumns() * GetNumLines();
}

void Environment::Clear(void)
{
	for (MapLine line : m_map)
	{
		line.clear();
	}
	m_map.clear();
	m_agent = {};
	m_isInitialized = m_hasStart = m_hasFinish = false;
}

bool Environment::StoreLineOnMap(const std::string& line)
{
	int numLines = GetNumLines();
	int numCols = GetNumColumns();

	int lineLen = static_cast<int>(line.length());
	
	bool validLine = false;
	if (lineLen >= MIN_LINE_SIZE)
	{
		if (numCols == 0U || lineLen == numCols)
		{
			validLine = true;
		}
	}

	if (!validLine)
	{
		std::string err = __func__;
		err += ": invalid line read. Line should be ";
		if (numCols == 0U)
		{
			err += "at least " + std::to_string(MIN_LINE_SIZE) + " chars long";
		}
		else
		{
			err += "equal to first line length (" + std::to_string(numCols) + " chars)";
		}

		fprintf(stderr, "%s\n", err.c_str());
		return false;		
	}

	MapLine newLine(lineLen);
	m_map.push_back(newLine);
	numCols = lineLen;

	for (int col = 0; col < numCols; ++col)
	{
		switch (line[col])
		{
		case '*':
			m_map[numLines][col].m_tileState = TileState::Unchecked_Blocked;
			break;
		case '.':
			m_map[numLines][col].m_tileState = TileState::Unchecked_Empty;
			break;
		case 'S': case 's':
			if (!m_hasStart)
			{
				m_map[numLines][col].m_tileState = TileState::Tile_Start;
				m_start = m_agent.m_currentPos = { numLines, col };
				m_hasStart = true;
			}
			else
			{
				fprintf(stderr, "%s: map data contains more than one Start location\n", __func__);
				return false;
			}
			break;
		case 'F': case 'f':
			if (!m_hasFinish)
			{
				m_map[numLines][col].m_tileState = TileState::Tile_Finish;
				m_finish = { numLines, col };
				m_hasFinish = true;
			}
			else
			{
				fprintf(stderr, "%s: map data contains more than one Finish location\n", __func__);
				return false;
			}
			break;
		default:
			fprintf(stderr, "%s: map data contains invalid char \"%c\"\n", __func__, line[col]);
			return false;
		}
	}

	return true;
}
