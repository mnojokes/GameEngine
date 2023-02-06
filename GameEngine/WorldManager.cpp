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

#include "WorldManager.h"
#include "Messenger.h"
#include "MessagesWorldManager.h"
#include "MessageFileRequest.h"
#include "StringUtility.h"

WorldManager::WorldManager(Messenger& messenger)
	: Subscriber(messenger)
	, Singleton(this)
	, m_persistentWorld(World())
	, m_activeWorld(World())
{
	FileRequestMessage frm;
	frm.m_extension = ".lvl";
	frm.m_FileLoader = std::bind(&WorldManager::ReadWorldData, this, std::placeholders::_1);
	m_messenger.Send(frm);
}

WorldManager::~WorldManager(void)
{
	/* Delete parsed data */
	for (std::pair<std::string, JSONDoc*> it : m_parsedWorlds)
	{
		delete it.second;
	}
}

void WorldManager::RegisterMessages(void)
{}

void WorldManager::ProcessMessage(const Message* message)
{}

void WorldManager::UpdateWorlds(float deltaTime)
{
	m_persistentWorld.Update(deltaTime);
	m_activeWorld.Update(deltaTime);
}

bool WorldManager::LoadWorld(WorldType toWorld, const std::string& filename)
{
	World* world = nullptr;
	switch (toWorld)
	{
	case WorldType::W_ACTIVE:
		world = &m_activeWorld;
		break;
	case WorldType::W_PERSISTENT:
		world = &m_persistentWorld;
		break;
	default:
		// should never get here
		fprintf(stderr, "WorldManager::%s: invalid world type passed\n", __func__);
		return false;		
	}

	bool result = false;
	std::map<std::string, JSONDoc*>::const_iterator mapIter = m_parsedWorlds.find(filename.c_str());
	if (mapIter == m_parsedWorlds.end())
	{
		fprintf(stderr, "WorldManager::%s: file \"%s\" not found\n", __func__, filename.c_str());
	}
	else
	{
		result = world->Deserialize(mapIter->second);
	}

	return result;
}

bool WorldManager::IsWorldDataAvailable(void) const
{
	return !m_parsedWorlds.empty();
}

World* WorldManager::GetActiveWorld(void)
{
	return &m_activeWorld;
}

void WorldManager::ReadWorldData(const std::string& filePath)
{
	std::string filename = StringUtility::ExtractFilenameFromPath(filePath);
	if (m_parsedWorlds.find(filename) == m_parsedWorlds.end())
	{
		JSONDoc* parsedWorldData = JSONUtility::ParseFile(filePath);
		m_parsedWorlds.insert(std::make_pair(filename, parsedWorldData));
	}
	else
	{
		fprintf(stderr, "WorldManager::%s: \"%s\" already exists\n", __func__, filename.c_str());
	}
}
