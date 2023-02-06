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

#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
#include "Matrix.h"
#include "Vector3.h"

class InputMessage;
class SceneComponent;
class Transform;

class Camera : public GameObject
{
public:
	static void Register(void);

	Camera(void);
	Camera(uint64_t id, World* parentWorld);
	~Camera(void);

	void Initialize(void) override;
	void Deserialize(const JSONData& source);

	GameObject* Clone(World* parentWorld) const override;

	void Update(float deltaTime) override;
	void ProcessInput(const InputMessage* msg);

	void SetPosition(const Vector3& position);
	void SetLookAt(const Vector3& target);

	const Matrix& GetViewMatrix(void) const;

	const std::string& GetObjectTypeName(void) const override;
	static const std::string& GetClassTypeName(void);

	void CollisionReact(void) override final {}

private:
	void CalculateRotation(Transform& transform);

private:
	SceneComponent* m_scene;
	Matrix m_view;
	Vector3 m_lookAtTarget;
};

#endif