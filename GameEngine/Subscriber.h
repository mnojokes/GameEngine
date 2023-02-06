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

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

class Messenger;
class Message;

// Subscriber: used as a base class for classes that will need to send/receive messages to/from others
class Subscriber
{
	friend class Messenger;
public:
	Subscriber(void) = delete;
	virtual ~Subscriber(void);

	virtual void RegisterMessages(void) {}
	virtual void ProcessMessage(const Message* message) {}

protected:
	Subscriber(Messenger& messenger);

protected:
	Messenger& m_messenger;

private:
	size_t m_bindingSlotIndex;
};

#endif
