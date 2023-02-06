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

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <string>
#include "JSONDefinitions.h"

class GameObject;
class JSONData;
class Camera;

/* World - contains objects from the currently active world in the game */
class World
{
public:
	World(void);
	~World(void);
	World(const World& rhs) = delete;

	bool Deserialize(JSONDoc* data);
	void Serialize(void);

	void Update(float deltaTime);

	void AddObject(GameObject* object);
	void RemoveObject(GameObject* object);

	void Destroy(void);
	// void SaveAndDestroy(void);

	Camera* GetCamera(void) const;

private:
	void OverrideObjectData(GameObject* object, JSONData data) const;

protected:
	/* World objects: game objects belonging to a particular world */
	std::vector<GameObject*> m_worldObjects;

private:
	Camera* m_camera;
};

#endif
