#include "stdafx.h"
#include "CameraScrollComponent.h"

#include "GameObjects/GameObject.h"
#include "Input/InputManager.h"

CameraScrollComponent::CameraScrollComponent()
{
	m_isTickable = true;
}

void CameraScrollComponent::Tick(float deltaTime)
{
	if (InputManager::GetInstance()->IsMiddleMousePressed())
	{
		const auto mouseDelta = InputManager::GetInstance()->GetMouseDelta();

		const Vector3D positionDelta = Vector3D(-mouseDelta.first * cameraSpeed, mouseDelta.second * cameraSpeed, 0.0f);

		GetOwner()->SetPosition(GetOwner()->GetPosition() + positionDelta);
	}
}
