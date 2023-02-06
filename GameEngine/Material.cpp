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

#include "Material.h"
#include "JSONData.h"
#include "Vector4.h"

Material::Material(void)
	: m_blend("NoBlend")
	, m_color(1.0f, 1.0f, 1.0f, 1.0f)
	, m_texture(nullptr)
{}

void Material::Deserialize(const JSONData& source)
{
	if (source.HasMember("Blend"))
	{
		source.GetString("Blend", m_blend);
	}

	if (source.HasMember("Color"))
	{
		source.GetColor("Color", m_color);
	}

	if (source.HasMember("Texture"))
	{
		source.GetString("Texture", m_textureName);
	}
}