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

#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "Component.h"
#include <functional>

class InputMessage;

class InputComponent : public Component
{
public:
	static void Register(void);

	InputComponent(void);
	InputComponent(uint64_t id, GameObject* owner);
	InputComponent(const InputComponent& rhs);
	~InputComponent(void);

	void RegisterCallback(std::function<void(const InputMessage*)>* CallbackFn);

	void RegisterMessages(void) override;
	void ProcessMessage(const Message* message) override;

	void Deserialize(const JSONData & source) override;

	Component* Clone(void) const override;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

private:
	std::function<void(const InputMessage*)> m_ParentCallback;
};

#endif
