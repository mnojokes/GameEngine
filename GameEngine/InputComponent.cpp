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

#include "InputComponent.h"
#include "GameObjectFactory.h"
#include "JSONData.h"
#include "Messenger.h"
#include "MessagesInput.h"

void InputComponent::Register(void)
{
	GameObjectFactory::Get()->RegisterComponent<InputComponent>();
}

InputComponent::InputComponent(void)
	: Component(0U, nullptr)
{}

InputComponent::InputComponent(uint64_t id, GameObject* owner)
	: Component(id, owner)
{}

InputComponent::InputComponent(const InputComponent& rhs)
	: Component(rhs)
{
	RegisterMessages();
}

InputComponent::~InputComponent(void)
{}

void InputComponent::RegisterCallback(std::function<void(const InputMessage*)>* fn)
{
	m_ParentCallback = *fn;
}

void InputComponent::RegisterMessages(void)
{
	m_messenger.RegisterMessage<InputMessage>(this);
}

void InputComponent::ProcessMessage(const Message* message)
{
	m_ParentCallback(static_cast<const InputMessage*>(message));
}

void InputComponent::Deserialize(const JSONData& source)
{}

Component* InputComponent::Clone(void) const
{
	Component* clone = new InputComponent(*this);
	return clone;
}

const std::string& InputComponent::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& InputComponent::GetClassTypeName(void)
{
	static const std::string name = "InputComponent";
	return name;
}
