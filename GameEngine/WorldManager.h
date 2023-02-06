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

#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include <map>
#include "Subscriber.h"
#include "Singleton.h"
#include "JSONUtility.h"
#include "World.h"

enum class WorldType
{
	W_ACTIVE,
	W_PERSISTENT,
};

class WorldManager : public Subscriber, public Singleton<WorldManager>
{
public:
	WorldManager(Messenger& messenger);
	~WorldManager(void);

	void RegisterMessages(void);
	void ProcessMessage(const Message* message);

	// Load data to a world from specified file
	bool LoadWorld(WorldType toWorld, const std::string& filename);
	bool IsWorldDataAvailable(void) const;

	void UpdateWorlds(float deltaTime);

	World* GetActiveWorld(void);

private:
	void ReadWorldData(const std::string& file);

private:
	/* World containing persistent objects */
	World m_persistentWorld;
	/* Currently active game world */
	World m_activeWorld;

	std::map<std::string, JSONDoc*> m_parsedWorlds;	// stores parsed JSON data
};

#endif
