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

#include "GameObjectFactory.h"
#include "JSONData.h"
#include "Messenger.h"
#include "MessageFileRequest.h"
#include "StringUtility.h"
#include "World.h"

GameObjectFactory::GameObjectFactory(Messenger& messenger)
	: Subscriber(messenger)
	, Singleton(this)
	, m_IDCounter(1U)	// ID starts at 1. 0 Reserved for dummy objects.
{
	FileRequestMessage frm;
	frm.m_extension = ".go";
	frm.m_FileLoader = std::bind(&GameObjectFactory::ReadGameObjectData, this, std::placeholders::_1);
	m_messenger.Send(frm);
}

GameObjectFactory::~GameObjectFactory(void)
{
	for (std::pair<std::string, JSONDoc*> it : m_parsedObjects)
	{
		delete it.second;
	}

	for (GameObject* obj : m_createdObjects)
	{
		delete obj;
	}
}

void GameObjectFactory::RegisterMessages(void)
{}

void GameObjectFactory::ProcessMessage(const Message* message)
{}

const std::vector<GameObject*>& GameObjectFactory::GetObjectList(void) const
{
	return m_createdObjects;
}

GameObject* GameObjectFactory::CreateNewObject(const std::string& type, World* parentWorld)
{
	GameObject* newObject = CreateNewObject_Internal(type, parentWorld);

	return newObject;
}

void GameObjectFactory::DeleteObject(GameObject* object)
{
	World* parentWorld = object->GetParentWorld();
	parentWorld->RemoveObject(object);

	std::vector<GameObject*>::iterator objectIter = m_createdObjects.begin();
	for (; objectIter < m_createdObjects.end(); ++objectIter)
	{
		if (object == *objectIter)
		{
			delete object;
			m_createdObjects.erase(objectIter);
			break;
		}
	}
}

GameObject* GameObjectFactory::Clone(const GameObject* source)
{
	GameObject* newObject = source->Clone(const_cast<World*>(source->GetParentWorld()));
	m_createdObjects.push_back(newObject);
	return newObject;
}

Component* GameObjectFactory::Clone(const Component* source) const
{
	Component* newComponent = source->Clone();
	return newComponent;
}

uint64_t GameObjectFactory::GenerateID(void)
{
	return m_IDCounter++;
}

Messenger& GameObjectFactory::GetMessenger(void) const
{
	return m_messenger;
}

void GameObjectFactory::ReadGameObjectData(const std::string& filePath)
{
	std::string filename = StringUtility::ExtractFilenameFromPath(filePath);
	if (m_parsedObjects.find(filename) == m_parsedObjects.end())
	{
		JSONDoc* parsedWorldData = JSONUtility::ParseFile(filePath);
		m_parsedObjects.insert(std::make_pair(filename, parsedWorldData));
	}
	else
	{
		fprintf(stderr, "GameObjectFactory::%s: \"%s\" already exists\n", __func__, filename.c_str());
	}
}

GameObject* GameObjectFactory::CreateNewObject_Internal(const std::string& type, World* parentWorld)
{
	std::map<std::string, GameObject*>::const_iterator mapIter = m_objectTypeRegister.find(type);
	if (mapIter == m_objectTypeRegister.end())
	{
		fprintf(stderr, "GameObjectFactory: \"%s\" type not registered\n", type.c_str());
		return nullptr;
	}

	GameObject* newObject = mapIter->second->Clone(parentWorld);

	/* Locate parsed file for reading data */
	std::string filename = type + FILEEXTENSION_GAMEOBJECT;
	std::map<std::string, JSONDoc*>::const_iterator it = m_parsedObjects.find(filename);
	if (it == m_parsedObjects.end())
	{
		fprintf(stderr, "GameObjectFactory: \"%s\" file not found\n", filename.c_str());
	}
	else
	{
		JSONData typeData = JSONData(it->second, type);
		if (!typeData.IsValid())
		{
			fprintf(stderr, "GameObjectFactory: file \"%s\" does not contain \"%s\" type data\n", filename.c_str(), type.c_str());
		}
		else
		{
			Deserialize(newObject, typeData);
		}
	}

	m_createdObjects.push_back(newObject);
	parentWorld->AddObject(newObject);

	return newObject;
}

void GameObjectFactory::Deserialize(GameObject* object, const JSONData& data)
{
	object->Deserialize(data);

	if (data.HasMember("children"))
	{
		World* parentWorld = object->GetParentWorld();
		JSONDoc::ConstValueIterator iter = data.GetArrayValueIterator("children");
		while (data.IsValidIterator(iter, "children"))
		{
			GameObject* newChild = nullptr;
			JSONData childData = JSONData(iter->MemberBegin());
			std::string childType = childData.GetName();
			childType = StringUtility::RemoveIndexFromName(childType);

			std::map<std::string, GameObject*>::const_iterator typeIter = m_objectTypeRegister.find(childType);
			if (typeIter == m_objectTypeRegister.end())
			{
				fprintf(stderr, "GameObjectFactory::%s: \"%s\" type not registered\n", __func__, childType.c_str());
			}
			else
			{
				newChild = typeIter->second->Clone(parentWorld);
				Deserialize(newChild, childData);
			}

			if (newChild != nullptr)
			{
				newChild->m_parent = object;
				object->m_children.push_back(newChild);
			}
			++iter;
		}
	}

	JSONDoc::ConstValueIterator iter = data.GetArrayValueIterator("components");
	while (data.IsValidIterator(iter, "components"))
	{
		JSONData compData = JSONData(iter->MemberBegin());
		std::string compName = compData.GetName();
		compName = StringUtility::RemoveIndexFromName(compName);

		std::map<std::string, Component*>::iterator componentArchetypeIter = m_componentTypeRegister.find(compName);
		if (componentArchetypeIter != m_componentTypeRegister.end())
		{
			Component* component = componentArchetypeIter->second->Clone();
			component->SetOwner(object);
			Deserialize(component, compData);
			object->AddComponent(component);
		}
		else
		{
			fprintf(stderr, "GameObjectFactory::%s: component type \"%s\" not registered\n", __func__, compName.c_str());
		}
		++iter;
	}

	object->Initialize();
}

void GameObjectFactory::Deserialize(Component* component, const JSONData& data)
{
	component->Deserialize(data);
}
