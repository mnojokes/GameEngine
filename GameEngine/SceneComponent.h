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

#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include "Component.h"
#include "Transform.h"

class GameObject;

class SceneComponent : public Component
{
public:
	static void Register(void);

	SceneComponent(void);
	SceneComponent(uint64_t id, GameObject* owner);
	SceneComponent(const SceneComponent& rhs) = default;

	void Deserialize(const JSONData& source) override;

	Component* Clone(void) const override;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

	// Returns references to object's transform data
	Transform& GetTransform(void);
	const Transform& GetTransform(void) const;

	// Set new transform data to the object
	void SetTransform(const Matrix& transform);

private:
	// Object's transform data
	Transform m_transform;
};

#endif
