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

#include "GraphicsComponent.h"
#include "GameObjectFactory.h"
#include "Message.h"
#include "Messenger.h"
#include "Material.h"
#include "MessagesGraphicsSystem.h"
#include "JSONData.h"

void GraphicsComponent::Register(void)
{
	GameObjectFactory::Get()->RegisterComponent<GraphicsComponent>();
}

GraphicsComponent::GraphicsComponent(void)
	: Component(0U, nullptr)
	, m_material(nullptr)
	, m_model(nullptr)
{}

GraphicsComponent::GraphicsComponent(uint64_t id, GameObject* owner)
	: Component(id, owner)
	, m_material(nullptr)
	, m_model(nullptr)
{}

GraphicsComponent::~GraphicsComponent(void)
{
	if (m_material != nullptr)
	{
		delete m_material;
		m_material = nullptr;
	}
}

void GraphicsComponent::Deserialize(const JSONData& source)
{
	std::string resourceName;

	if (source.HasMember("Material"))
	{
		source.GetString("Material", resourceName);
		RequestMaterial(resourceName);
	}

	if (source.HasMember("Model"))
	{
		source.GetString("Model", resourceName);
		RequestModel(resourceName);
	}
}

Component* GraphicsComponent::Clone(void) const
{
	Component* clone = new GraphicsComponent(*this);
	return clone;
}

const std::string& GraphicsComponent::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& GraphicsComponent::GetClassTypeName(void)
{
	static const std::string name = "GraphicsComponent";
	return name;
}

void GraphicsComponent::RequestMaterial(const std::string& materialName)
{
	MaterialRequestMessage msg(materialName, *this);
	m_messenger.Send(msg);
}

void GraphicsComponent::RequestModel(const std::string& modelName)
{
	ModelRequestMessage msg(modelName, *this);
	m_messenger.Send(msg);
}
