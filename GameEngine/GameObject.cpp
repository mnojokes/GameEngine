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

#include "GameObject.h"
#include "GameObjectFactory.h"

GameObject::GameObject(uint64_t id, World* parentWorld)
	: m_id(id)
	, m_parentWorld(parentWorld)
	, m_parent(nullptr)
{}

GameObject::GameObject(const GameObject& rhs)
	: m_id(GameObjectFactory::Get()->GenerateID())
	, m_parentWorld(rhs.m_parentWorld)
	, m_parent(rhs.m_parent)
{
	GameObjectFactory* GOF = GameObjectFactory::Get();
	for (GameObject* rhsChild : rhs.m_children)
	{
		GameObject* childCopy = GOF->Clone(rhsChild);
		m_children.push_back(childCopy);
	}
	for (Component* rhsComp : rhs.m_components)
	{
		Component* copy = GameObjectFactory::Get()->Clone(rhsComp);
		m_components.push_back(copy);
	}
}

GameObject::~GameObject(void)
{
	/* Destroy object's children */
	for (GameObject* child : m_children)
	{
		delete child;
	}

	/* Destroy object's components */
	for (Component* comp : m_components)
	{
		delete comp;
	}

	if (m_parent != nullptr)
	{
		/* Removes object from parent */
		for (int i = 0; i < m_parent->m_children.size(); ++i)
		{
			if (m_parent->m_children[i] == this)
			{
				m_parent->m_children.erase(m_parent->m_children.begin() + i);
				break;
			}
		}
	}
}

uint64_t GameObject::GetID(void) const
{
	return m_id;
}

World* GameObject::GetParentWorld(void)
{
	return m_parentWorld;
}

const World* GameObject::GetParentWorld(void) const
{
	return m_parentWorld;
}

void GameObject::QueryComponents(const std::string& type, std::vector<Component*>& list) const
{
	for (Component* comp : m_components)
	{
		if (comp->GetObjectTypeName() == type)
		{
			list.push_back(comp);
		}
	}
}

void GameObject::QueryChildren(const std::string& type, std::vector<GameObject*>& list) const
{
	for (GameObject* obj : m_children)
	{
		if (obj->GetObjectTypeName() == type)
		{
			list.push_back(obj);
		}
	}
}

void GameObject::AddComponent(Component* component)
{ 
	m_components.push_back(component);
}

void GameObject::CollisionReact(void)
{}
