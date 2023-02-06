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

#ifndef GRAPHICSCOMPONENT_H
#define GRAPHICSCOMPONENT_H

#include "Component.h"

class Material;
class Model;

class GraphicsComponent : public Component
{
public:
	GraphicsComponent(void);
	GraphicsComponent(uint64_t id, GameObject* owner);
	GraphicsComponent(const GraphicsComponent& rhs) = default;
	~GraphicsComponent(void);

	static void Register(void);

	void Deserialize(const JSONData& source) override;

	Component* Clone(void) const override;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

	// Sends a message requesting material/mesh from GraphicsSystem
	void RequestMaterial(const std::string& materialName);
	void RequestModel(const std::string& modelName);

public:
	Material* m_material;
	Model* m_model;
};

#endif
