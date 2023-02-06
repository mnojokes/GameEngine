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

#ifndef PHYSICSYSTEM_H
#define PHYSICSYSTEM_H

#include <vector>
#include "Transform.h"
#include "ISystem.h"

class GameObject;
class PhysicsComponent;
class SceneComponent;

class PhysicsSystem : public ISystem
{
public:
	PhysicsSystem(App* app, GameObjectFactory* GOF);
	~PhysicsSystem(void);

	bool Initialize(INIReader* ini) override final;
	void Update(float deltaTime) override final;
	void Exit(void) override final;

	void RegisterMessages(void);
	void ProcessMessage(const Message* message) override final;

	void RegisterComponents(void) const override final;

private:
	Matrix AssembleNewMatrix(const SceneComponent* sourceScene, float deltaTime,
		bool omitParentScale = false, bool omitParentRotation = false, bool omitParentPosition = false) const;

private:
	std::vector<PhysicsComponent*> m_physicsComponents;
	std::vector<SceneComponent*> m_sceneComponents;
};

#endif
