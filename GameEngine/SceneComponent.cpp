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

#include "SceneComponent.h"
#include "GameObjectFactory.h"
#include "JSONData.h"

void SceneComponent::Register(void)
{
	GameObjectFactory::Get()->RegisterComponent<SceneComponent>();
}

SceneComponent::SceneComponent(void)
	: Component(0U, nullptr)
{}

SceneComponent::SceneComponent(uint64_t id, GameObject* owner)
	: Component(id, owner)
{}

void SceneComponent::Deserialize(const JSONData& source)
{
	JSONData memberData = source.FindMember("transform");
	if (memberData.IsValid())
	{
		m_transform.Deserialize(memberData);
	}
}

Component* SceneComponent::Clone(void) const
{
	Component* clone = new SceneComponent(*this);
	return clone;
}

const std::string& SceneComponent::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& SceneComponent::GetClassTypeName(void)
{
	static const std::string name = "SceneComponent";
	return name;
}

Transform& SceneComponent::GetTransform(void)
{
	return m_transform;
}

const Transform& SceneComponent::GetTransform(void) const
{
	return m_transform;
}

void SceneComponent::SetTransform(const Matrix& transform)
{
	m_transform.SetTransformMatrix(transform);
}
