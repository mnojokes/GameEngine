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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <typeinfo>

#define MESSAGE_CTOR(x, ...) x(__VA_ARGS__) : Message(typeid(x).hash_code())

class Message
{
public:
	template <typename Type>
	bool IsType(void) const
	{
		return m_hash == typeid(Type).hash_code();
	}

	bool IsType(size_t hash) const
	{
		return m_hash == hash;
	}

protected:
	explicit Message(size_t hash)
		: m_hash(hash)
	{}

private:
	size_t m_hash;
};

#endif