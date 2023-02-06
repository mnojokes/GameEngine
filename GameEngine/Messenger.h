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

#ifndef MESSENGER_H
#define MESSENGER_H

#include "Message.h"
#include "Subscriber.h"
#include <vector>

class Messenger
{
public:
	Messenger();
	~Messenger();

	template <typename MessageType>
	void RegisterMessage(Subscriber* subscriber);
	template <typename MessageType>
	void Send(const MessageType& message);

	// Remove subscriber from messenger bindings
	void Unsubscribe(Subscriber* subscriber);

	void Dispatch(void);

private:
	struct MessageBindings
	{
		Subscriber* m_subscriber = nullptr;
		std::vector<size_t> msgHashes;
	};

	std::vector<MessageBindings> m_messageBindings;
	std::vector<size_t> m_availableBindings;

	static const uint8_t NUM_QUEUES = 2U;
	std::vector<Message*> m_messages[NUM_QUEUES];
	std::vector<Message*>* m_activeQueue = m_messages;
};

template <typename MessageType>
void Messenger::RegisterMessage(Subscriber* subscriber)
{
	if (subscriber != nullptr)
	{
		size_t messageHash = typeid(MessageType).hash_code();

		size_t useBindingIndex = subscriber->m_bindingSlotIndex;
		if (useBindingIndex == UINT64_MAX)	// no binding for this subscriber
		{
			MessageBindings newBinding;
			newBinding.m_subscriber = subscriber;
			newBinding.msgHashes.push_back(messageHash);

			if (!m_availableBindings.empty())
			{
				subscriber->m_bindingSlotIndex = useBindingIndex = m_availableBindings.back();
				m_availableBindings.pop_back();

				m_messageBindings[useBindingIndex] = newBinding;
			}
			else
			{
				subscriber->m_bindingSlotIndex = m_messageBindings.size();
				m_messageBindings.push_back(newBinding);
			}
		}
		else
		{
			std::vector<size_t>& existingHashes = m_messageBindings[useBindingIndex].msgHashes;
			for (size_t hash : existingHashes)
			{
				if (hash == messageHash) // already registered
				{
					return;
				}
			}
			existingHashes.push_back(messageHash);
		}
	}
	else
	{
		fprintf(stderr, "Messenger::%s: subscriber was nullptr\n", __func__);
	}
}

template <typename MessageType>
void Messenger::Send(const MessageType& message)
{
	Message* messagePtr = new MessageType(message);
	
	m_activeQueue->push_back(messagePtr);
}

#endif
