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

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include "FileSystem.h"

class JSONData;
class GameObject;

class Component : public Subscriber
{
public:
	friend class GameObjectFactory;
	friend class GameObject;
	friend class World;

	bool IsInitialized(void) const;
	void SetInitialized(bool status);

	GameObject* GetOwner(void) const;
	void SetOwner(GameObject* owner);

protected:
	Component(uint64_t id, GameObject* owner);
	Component(const Component& rhs);
	virtual ~Component(void);

	virtual void Deserialize(const JSONData& source) = 0;

	virtual Component* Clone(void) const = 0;

	virtual const std::string& GetObjectTypeName(void) const = 0;

protected:
	GameObject* m_owner;

private:
	uint64_t m_id;
	bool m_isInitialized;
};

#endif
