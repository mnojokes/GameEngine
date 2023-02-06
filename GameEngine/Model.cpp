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

#include "Model.h"
#include "JSONData.h"

Model::Model(void)
	: m_cullMode(CullMode::None)
	, m_vertexBuffer(nullptr)
{}

Model::~Model(void)
{
	SAFE_RELEASE(m_vertexBuffer);
}

void Model::Deserialize(const JSONData& source)
{
	if (source.HasMember("VertexLayout"))
	{
		source.GetString("VertexLayout", m_vertexLayout);
	}

	if (source.HasMember("CullMode"))
	{
		std::string cullMode;
		source.GetString("CullMode", cullMode);
		if (cullMode == "Back")
		{
			m_cullMode = CullMode::Back;
		}
		else if (cullMode == "Front")
		{
			m_cullMode = CullMode::Front;
		}
		else if (cullMode == "None")
		{
			m_cullMode = CullMode::None;
		}
	}

	if (source.HasMember("Mesh"))
	{
		source.GetString("Mesh", m_mesh);
	}
}
