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

#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "ThirdParty/INIReader/cpp/INIReader.h"
#include "Subscriber.h"

class App;
class GameObjectFactory;

class ISystem : public Subscriber
{
public:
	ISystem(void) = delete;
	virtual ~ISystem(void)
	{}

	virtual bool Initialize(INIReader* ini) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Exit(void) = 0;

	virtual void RegisterComponents(void) const {}

	const INIReader* m_iniReader;

protected:
	ISystem(App* app, GameObjectFactory* GOF);

protected:
	App* m_appPtr;
	GameObjectFactory* m_GOF;
};

#endif
