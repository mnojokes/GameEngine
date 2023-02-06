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

#ifndef MESSAGESGRAPHICSSYSTEM_H
#define MESSAGESGRAPHICSSYSTEM_H

#include "Message.h"
#include <string>

class GraphicsComponent;
class Material;

class WindowResizeMessage : public Message
{
public:
	MESSAGE_CTOR(WindowResizeMessage,
		uint32_t width = 0U,
		uint32_t height = 0U)
		, m_width(width)
		, m_height(height)
	{}

public:
	uint32_t m_width;
	uint32_t m_height;
};

class MaterialRequestMessage : public Message
{
public:
	MESSAGE_CTOR(MaterialRequestMessage,
		const std::string& materialName,
		GraphicsComponent& componentRef)
		, m_materialName(materialName)
		, m_componentRef(componentRef)
	{}

public:
	std::string m_materialName;
	GraphicsComponent& m_componentRef;
};

class ModelRequestMessage : public Message
{
public:
	MESSAGE_CTOR(ModelRequestMessage,
		const std::string& modelName,
		GraphicsComponent& componentRef)
		, m_modelName(modelName)
		, m_componentRef(componentRef)
	{}

public:
	std::string m_modelName;
	GraphicsComponent& m_componentRef;
};

class AssignTextureToMaterialMessage : public Message
{
public:
	MESSAGE_CTOR(AssignTextureToMaterialMessage,
		const std::string& materialName,
		const std::string& textureName)
		, m_materialName(materialName)
		, m_textureName(textureName)
	{}

public:
	std::string m_materialName;
	std::string m_textureName;
};

class AssignMeshToModelMessage : public Message
{
public:
	MESSAGE_CTOR(AssignMeshToModelMessage,
		const std::string& modelName,
		const std::string& meshName)
		, m_modelName(modelName)
		, m_meshName(meshName)
	{}

public:
	std::string m_modelName;
	std::string m_meshName;
};

#endif
