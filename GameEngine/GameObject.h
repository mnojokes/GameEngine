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

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include "Component.h"

class JSONData;
class World;

class GameObject
{
public:
	GameObject(uint64_t id, World* parentWorld);
	GameObject(const GameObject& rhs);
	virtual ~GameObject(void);

	virtual void Deserialize(const JSONData& source) = 0;
	virtual GameObject* Clone(World* parentWorld) const = 0;

	virtual void Initialize(void) {}
	virtual void Update(float deltaTime) = 0;

	uint64_t GetID(void) const;
	virtual const std::string& GetObjectTypeName(void) const = 0;
	World* GetParentWorld(void);
	const World* GetParentWorld(void) const;

	// QueryComponents: lists object's components of a particular type
	void QueryComponents(const std::string& type, std::vector<Component*>& list) const;
	template <typename Type>
	void QueryComponents(std::vector<Type*>& list) const;

	// QueryChildren: lists object's children of a particular type
	void QueryChildren(const std::string& type, std::vector<GameObject*>& list) const;

	void AddComponent(Component* component);

	virtual void CollisionReact(void); // object's default reaction to collision

public:
	// Pointer to a parent object
	GameObject* m_parent;
	// Vector of pointers to children objects
	std::vector<GameObject*> m_children;

protected:
	// Pointer to object's parent world
	World* m_parentWorld;

private:
	// Object's unique id
	const uint64_t m_id;
	// Vector of object's components
	std::vector<Component*> m_components;
};

template <typename Type>
void GameObject::QueryComponents(std::vector<Type*>& list) const
{
	std::string type = Type::GetClassTypeName();

	for (Component* comp : m_components)
	{
		if (comp->GetObjectTypeName() == type)
		{
			list.push_back(static_cast<Type*>(comp));
		}
	}
}

#endif
