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

#include "Camera.h"
#include "GameObjectFactory.h"
#include "InputComponent.h"
#include "SceneComponent.h"
#include "MathConstants.h"
#include "Messenger.h"
#include "MessagesInput.h"
#include "World.h"

void Camera::Register(void)
{
	GameObjectFactory::Get()->RegisterObject<Camera>();
}

Camera::Camera(void)
	: GameObject(0U, nullptr)
	, m_scene(nullptr)
	, m_lookAtTarget({ 0.0f, 0.0f, 1.0f })
{
	m_view = Matrix::LookAtLH({ 0.0f, 0.0f, 0.0f }, m_lookAtTarget, { 0.0f, 1.0f, 0.0f });
}

Camera::Camera(uint64_t id, World* parentWorld)
	: GameObject(id, parentWorld)
	, m_scene(nullptr)
	, m_lookAtTarget({ 0.0f, 0.0f, 1.0f })
{
	m_view = Matrix::LookAtLH({ 0.0f, 0.0f, 0.0f }, m_lookAtTarget, { 0.0f, 1.0f, 0.0f });
}

Camera::~Camera(void)
{}

void Camera::Initialize(void)
{
	std::vector<Component*> comp;
	QueryComponents("InputComponent", comp);
	for (Component* iter : comp)
	{
		InputComponent* inputIter = static_cast<InputComponent*>(iter);
		std::function<void(const InputMessage*)> fn = std::bind(&Camera::ProcessInput, this, std::placeholders::_1);
		inputIter->RegisterCallback(&fn);
	}
	comp.clear();
	QueryComponents("SceneComponent", comp);
	if (comp.empty())
	{
		fprintf(stderr, "Camera::%s: camera does not have a valid scene component\n", __func__);
		return;
	}
	m_scene = static_cast<SceneComponent*>(*comp.begin());
	SetPosition(m_scene->GetTransform().GetPosition());
}

void Camera::Deserialize(const JSONData& source)
{
	// TO DO
}

GameObject* Camera::Clone(World* parentWorld) const
{
	GameObject* clone = new Camera(GameObjectFactory::Get()->GenerateID(), parentWorld);
	return clone;
}

void Camera::Update(float deltaTime)
{
	// TO DO
}

void Camera::ProcessInput(const InputMessage* msg)
{
	// TO DO
}

void Camera::SetPosition(const Vector3& position)
{
	Transform& transform = m_scene->GetTransform();
	transform.SetPosition(position);
	CalculateRotation(transform);
	m_view = Matrix::LookAtLH(position, m_lookAtTarget, { 0.0f, 1.0f, 0.0f });
}

void Camera::SetLookAt(const Vector3& target)
{
	Transform& transform = m_scene->GetTransform();
	m_lookAtTarget = target;
	CalculateRotation(transform);
	m_view = Matrix::LookAtLH(transform.GetPosition(), m_lookAtTarget, { 0.0f, 1.0f, 0.0f });
}

const Matrix& Camera::GetViewMatrix(void) const
{
	return m_view;
}

const std::string& Camera::GetObjectTypeName(void) const
{
	return GetClassTypeName();
}

const std::string& Camera::GetClassTypeName(void)
{
	static const std::string name = "Camera";
	return name;
}

void Camera::CalculateRotation(Transform& transform)
{
	Vector3 direction = m_lookAtTarget - transform.GetPosition();
	direction = direction.Normalize();

	// Get angle in radians
	float x = asin(direction.y);
	float y = atan2(direction.x, direction.z);
	float z = transform.GetRotation().z;

	// Convert radians to degrees
	x *= 180.0f * rcpPI;
	y *= 180.0f * rcpPI;

	Vector3 rotation(x, y, z);
	transform.SetRotation(rotation);
}
