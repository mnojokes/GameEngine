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

#include "Component.h"
#include "GameObjectFactory.h"

Component::Component(uint64_t id, GameObject* owner)
	: Subscriber(GameObjectFactory::Get()->GetMessenger())
	, m_id(id)
	, m_isInitialized(false)
	, m_owner(owner)
{}

Component::Component(const Component& rhs)
	: Subscriber(rhs.m_messenger)
	, m_id(GameObjectFactory::Get()->GenerateID())
	, m_isInitialized(rhs.m_isInitialized)
	, m_owner(rhs.m_owner)
{}

Component::~Component(void)
{}

bool Component::IsInitialized(void) const
{
	return m_isInitialized;
}

void Component::SetInitialized(bool status)
{
	m_isInitialized = status;
}

GameObject* Component::GetOwner(void) const
{
	return m_owner;
}

void Component::SetOwner(GameObject* owner)
{
	m_owner = owner;
}
