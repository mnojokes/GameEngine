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

#include "Messenger.h"

Messenger::Messenger(void)
{}

Messenger::~Messenger(void)
{}

void Messenger::Unsubscribe(Subscriber* subscriber)
{
	if (subscriber != nullptr)
	{
		size_t index = subscriber->m_bindingSlotIndex;
		if (index != UINT64_MAX)
		{
			m_messageBindings[index].m_subscriber = nullptr;
			m_messageBindings[index].msgHashes.clear();
			m_availableBindings.push_back(index);
		}
	}
	else
	{
		fprintf(stderr, "Messenger::%s: subscriber was nullptr\n", __func__);
	}
}

void Messenger::Dispatch(void)
{
	std::vector<Message*>* sendfromQueue = m_activeQueue;
	size_t newQueueIndex = (m_activeQueue - m_messages + 1U) % NUM_QUEUES;
	m_activeQueue = &m_messages[newQueueIndex];

	for (Message* message : *sendfromQueue)
	{
		for (MessageBindings& messageBinding : m_messageBindings)
		{
			for (size_t subscribedHash : messageBinding.msgHashes)
			{
				if (message->IsType(subscribedHash))
				{
					messageBinding.m_subscriber->ProcessMessage(message);
				}
			}
		}

		delete message;
	}

	sendfromQueue->clear();
}
