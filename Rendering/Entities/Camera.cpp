#include "stdafx.h"
#include "Camera.h"

#include "Archetypes/ArchetypeBuilder.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"

namespace Rendering
{
	Camera* Camera::m_camera;

	Camera::Camera()
	{
		m_camera = this;
	}

	Camera* Camera::Get()
	{
		return m_camera;
	}

	void Camera::DefineArchetype(ArchetypeBuilder& builder)
	{
		Entity::DefineArchetype(builder);

		builder.AddComponent<CameraComponent>();
		builder.AddComponent<TransformComponent>();
	}
}
