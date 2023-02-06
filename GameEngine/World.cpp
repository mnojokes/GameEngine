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

#include "World.h"
#include "GameObjectFactory.h"
#include "Camera.h"
#include "JSONData.h"
#include "StringUtility.h"

World::World(void)
	: m_camera(nullptr)
{}

World::~World(void)
{
	Destroy();
}

bool World::Deserialize(JSONDoc* data)
{
	GameObjectFactory* GOF = GameObjectFactory::Get();
	GameObject* newObject = nullptr;

	/* Check if file is properly formated and contains valid world data */
	JSONData worldData = JSONData(data->MemberBegin());
	std::string filename = worldData.GetName();

	JSONData worldObjects = JSONData(worldData.FindMember("WorldObjects"));
	if (!worldObjects.IsValid())
	{
		fprintf(stderr, "World::%s: file \"%s\" does not contain valid world object data\n", __func__, filename.c_str());
		return false;
	}

	/* Populate m_worldObjects */
	JSONDoc::ConstValueIterator iter = worldObjects.GetArrayBegin();
	JSONDoc::ConstValueIterator arrayEnd = worldObjects.GetArrayEnd();
	while (iter != arrayEnd)
	{
		JSONData objectData = JSONData(iter->MemberBegin());
		std::string type = objectData.GetName();
		newObject = GOF->CreateNewObject(type.c_str(), this);

		if (newObject != nullptr)
		{	// check for world-specific object overrides
			if (newObject->GetObjectTypeName() == Camera::GetClassTypeName())
			{
				m_camera = static_cast<Camera*>(newObject);
			}
			OverrideObjectData(newObject, objectData);
		}
		++iter;
	}

	return true;
}

void World::Serialize(void)
{
	// TO DO
}

void World::Update(float deltaTime)
{
	for (GameObject* obj : m_worldObjects)
	{
		obj->Update(deltaTime);
	}
}

void World::AddObject(GameObject* object)
{
	m_worldObjects.push_back(object);
}

void World::RemoveObject(GameObject* object)
{
	std::vector<GameObject*>::const_iterator iter = m_worldObjects.begin();
	while (iter < m_worldObjects.end())
	{
		if (*iter == object)
		{
			m_worldObjects.erase(iter);
			break;
		}
		++iter;
	}
}

void World::Destroy(void)
{
	GameObjectFactory* GOF = GameObjectFactory::Get();

	while (!m_worldObjects.empty())
	{
		GOF->DeleteObject(*m_worldObjects.begin());
	}

	m_camera = nullptr;
}

Camera* World::GetCamera(void) const
{
	return m_camera;
}

void World::OverrideObjectData(GameObject* object, JSONData data) const
{
	object->Deserialize(data);
	
	if (data.HasMember("children"))
	{
		JSONDoc::ConstValueIterator iter = data.GetArrayValueIterator("children");
		while (data.IsValidIterator(iter, "children"))
		{
			JSONData childData = JSONData(iter->MemberBegin());
			std::string childType = childData.GetName();
			uint64_t childIndex = StringUtility::GetIndexFromName(childType);
			childType = StringUtility::RemoveIndexFromName(childType);
			std::vector<GameObject*> list;
			object->QueryChildren(childType, list);

			if (!list.empty())
			{
				if (childIndex < list.size())
				{
					GameObject* child = list[childIndex];
					OverrideObjectData(child, childData);
				}
				else
				{
					fprintf(stderr, "World::%s: object \"%s\" with index \"%lld\" not found\n",
					__func__, childType.c_str(), childIndex);
				}
			}
			else
			{
				fprintf(stderr, "World::%s: object \"%s\" does not have child \"%s\"\n",
				__func__, object->GetObjectTypeName().c_str(), childType.c_str());
			}
			++iter;
		}
	}

	if (data.HasMember("components"))
	{
		JSONDoc::ConstValueIterator iter = data.GetArrayValueIterator("components");
		while (data.IsValidIterator(iter, "components"))
		{
			JSONData componentData = JSONData(iter->MemberBegin());
			std::string componentType = componentData.GetName();
			uint64_t componentIndex = StringUtility::GetIndexFromName(componentType);
			componentType = StringUtility::RemoveIndexFromName(componentType);
			std::vector<Component*> list;
			object->QueryComponents(componentType, list);

			if (!list.empty())
			{
				if (componentIndex < list.size())
				{
					Component* component = list[componentIndex];
					component->Deserialize(componentData);
				}
				else
				{
					fprintf(stderr, "World::%s: component \"%s\" with index \"%lld\" not found with object \"%s\"\n",
						__func__, componentType.c_str(), componentIndex, object->GetObjectTypeName().c_str());
				}
			}
			else
			{
				fprintf(stderr, "World::%s: object \"%s\" does not have component \"%s\"\n",
					__func__, object->GetObjectTypeName().c_str(), componentType.c_str());
			}
			++iter;
		}
	}

	object->Initialize();
}
