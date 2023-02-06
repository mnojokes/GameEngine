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

#ifndef VERTEXDECLARATIONS_H
#define VERTEXDECLARATIONS_H

#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"

struct VertexColor
{
	Vector3 m_position;
	uint8_t m_padding0[4];
	Color m_color;
};

struct VertexTexCoord
{
	Vector3 m_position;
	uint8_t m_padding0[4];
	Vector2 m_texCoord;
};

struct VertexTexCoordColor
{
	Vector3 m_position;
	uint8_t m_padding0[4];
	Color m_color;
	Vector2 m_texCoord;
};

#endif
