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

#ifndef MESSAGESWORLDMANAGER_H
#define MESSAGESWORLDMANAGER_H

#include "Message.h"
#include <string>

/* Message containing filename of the world to be activated */
class ActivateWorldMessage : public Message
{
public:
	MESSAGE_CTOR(ActivateWorldMessage) {}

public:
	std::string m_filename;	// filename of world to be loaded
};

#endif