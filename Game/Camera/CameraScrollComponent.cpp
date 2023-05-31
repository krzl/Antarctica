#include "stdafx.h"
#include "CameraScrollComponent.h"

#include "GameObjects/GameObject.h"
#include "Input/InputSystem.h"

void CameraScrollComponent::Tick(float deltaTime)
{
	if (InputSystem::GetInstance()->IsMiddleMousePressed())
	{
		const auto mouseDelta = InputSystem::GetInstance()->GetMouseDelta();

		const Vector3D positionDelta = Vector3D(- mouseDelta.first * cameraSpeed, mouseDelta.second * cameraSpeed, 0.0f);

		GetOwner()->SetPosition(GetOwner()->GetPosition() + positionDelta);
	}
}
