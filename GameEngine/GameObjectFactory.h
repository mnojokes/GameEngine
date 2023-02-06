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

#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <map>
#include <vector>
#include <string>
#include "GameObject.h"
#include "Component.h"
#include "Singleton.h"
#include "Subscriber.h"
#include "JSONUtility.h"

#define FILEEXTENSION_GAMEOBJECT ".go"

class JSONData;

class GameObjectFactory : public Subscriber, public Singleton<GameObjectFactory>
{
public:
	GameObjectFactory(Messenger& messenger);
	~GameObjectFactory(void);

	void RegisterMessages(void);
	void ProcessMessage(const Message* message);

	const std::vector<GameObject*>& GetObjectList(void) const;
	
	template <typename Type>
	void GetComponentsOfType(std::vector<Type*>& list) const;

	GameObject* CreateNewObject(const std::string& type, World* parentWorld);
	template <typename Type>
	Type* CreateNewObject(World* parentWorld);

	void DeleteObject(GameObject* obj);

	GameObject* Clone(const GameObject* source);
	Component* Clone(const Component* source) const;

	uint64_t GenerateID(void);
	Messenger& GetMessenger(void) const;

	template <typename ObjType>
	void RegisterObject(void);
	template <typename CompType>
	void RegisterComponent(void);

private:
	void ReadGameObjectData(const std::string& filePath);

	GameObject* CreateNewObject_Internal(const std::string& type, World* parentWorld);

	void Deserialize(GameObject* object, const JSONData& data);
	void Deserialize(Component* component, const JSONData& data);

public:
	std::map<std::string, GameObject*> m_objectTypeRegister;	// stores object type names and data
	std::map<std::string, Component*> m_componentTypeRegister;	// stores component type names and data

private:
	std::map<std::string, JSONDoc*> m_parsedObjects;	// stores parsed JSON data
	uint64_t m_IDCounter;	// unique ID counter, 
	std::vector<GameObject*> m_createdObjects;	// stores objects that GOF creates
};

template <typename Type>
void GameObjectFactory::GetComponentsOfType(std::vector<Type*>& list) const
{
	list.clear();
	for (GameObject* obj : m_createdObjects)
	{
		obj->QueryComponents(list);
	}
}

template <typename Type>
Type* GameObjectFactory::CreateNewObject(World* parentWorld)
{
	std::string type = Type::GetClassTypeName();
	GameObject* newObject = CreateNewObject_Internal(type, parentWorld);
	if (newObject == nullptr)
	{
		return nullptr;
	}

	return static_cast<Type*>(newObject);
}

template <typename ObjType>
void GameObjectFactory::RegisterObject(void)
{
	m_objectTypeRegister.insert({ ObjType::GetClassTypeName(), new ObjType });
}

template <typename CompType>
void GameObjectFactory::RegisterComponent(void)
{
	m_componentTypeRegister.insert({ CompType::GetClassTypeName(), new CompType });
}

#endif
