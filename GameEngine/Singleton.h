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

#ifndef SINGLETON_H
#define SINGLETON_H

#include <typeinfo>
#include <stdio.h>

template <typename Class>
class Singleton
{
public:
	Singleton(Class* derivedObjPtr)
	{
		if (m_objStored == m_size)
		{
			fprintf(stderr, "%s: cannot create Singleton, storage limit reached\n", __func__);
			__debugbreak();
		}
		size_t id = typeid(Class).hash_code();

		for (uint8_t i = 0U; i < m_objStored; ++i)
		{
			if (m_storage[i].idHash == id)
			{
				fprintf(stderr, "%s: cannot create a duplicate singleton\n", __func__);
				__debugbreak();
			}
		}

		m_storage[m_objStored] = Pair{ id, derivedObjPtr };
		++m_objStored;
	}

	~Singleton(void)
	{
		size_t id = typeid(Class).hash_code();

		for (uint8_t index = 0U; index < m_objStored; ++index)
		{
			if (m_storage[index].idHash == id)
			{
				/* Object is not last: move all objects by one */
				while (index < m_objStored - 1)
				{	
					m_storage[index] = m_storage[index + 1];
					++index;
				}
				/* Last object: assign default value */
				m_storage[index] = {};
				break;
			}
		}
		--m_objStored;
	}

	static Class* Get(void)
	{
		size_t id = typeid(Class).hash_code();
		Class* ptr = nullptr;

		for (uint8_t i = 0U; i < m_objStored; ++i)
		{
			if (m_storage[i].idHash == id)
			{
				ptr = static_cast<Class*>(m_storage[i].classPtr);
				break;
			}
		}
		return ptr;
	}

private:
	struct Pair
	{
		size_t idHash;
		void* classPtr;
	};

	static const uint8_t m_size = 5U;
	static uint8_t m_objStored;
	static Pair m_storage[m_size];
};

template <typename Class>
uint8_t Singleton<Class>::m_objStored = 0U;

template <typename Class>
typename Singleton<Class>::Pair Singleton<Class>::m_storage[m_size] = {};

#endif
