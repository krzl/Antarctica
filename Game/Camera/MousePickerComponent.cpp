#include "stdafx.h"
#include "MousePickerComponent.h"

#include "Characters/Peasant.h"
#include "Core/Application.h"
#include "Entities/CameraComponent.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/World.h"
#include "Input/InputManager.h"

void MousePickerComponent::OnCreated()
{
	m_cameraComponent = GetOwner()->GetComponentRef<Rendering::CameraComponent>();

	m_inputHandle = InputManager::GetInstance()->OnLeftMouseButtonPressed.AddListener([this]()
	{
		const MousePosition pos = InputManager::GetInstance()->GetMousePosition();

		const float ndcX = (2.0f * pos.first) / Application::Get().GetWindow().GetWidth() - 1.0f;
		const float ndcY = 1.0f - (2.0f * pos.second) / Application::Get().GetWindow().GetHeight();

		const Vector4D clipCoords = Vector4D(ndcX, ndcY, 1.0f, 1.0f);

		Matrix4D inversePerspectiveMatrix = Inverse(m_cameraComponent->GetPerspectiveMatrix());
		Vector4D eyeCoordinates           = inversePerspectiveMatrix * clipCoords;
		eyeCoordinates /= eyeCoordinates.w;

		Matrix4D inverseViewMatrix = Inverse(m_cameraComponent->GetLookAtMatrix());
		Vector4D rayWorld          = inverseViewMatrix * eyeCoordinates;

		Vector3D direction = rayWorld.xyz - GetOwner()->GetPosition();
		direction          = direction.Normalize();

		Ray ray = { GetOwner()->GetPosition(), direction };

		Quadtree::TraceResult test = GetOwner()->GetWorld().GetQuadtree().TraceObject(ray);

		if (test.m_object)
		{
			if (test.m_object.Cast<Peasant>())
			{
				//test.m_object->Destroy();
			}
		}
	}, GetRef());
}
