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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "Color.h"

class JSONData;
class Texture2D;

class Material
{
public:
	Material(void);

	void Deserialize(const JSONData& source);

public:
	std::string m_blend;
	Color m_color;
	std::string m_textureName;
	Texture2D* m_texture;
};

#endif
