#include "stdafx.h"
#include "RTSCamera.h"

#include "CameraScrollComponent.h"
#include "Archetypes/ArchetypeBuilder.h"
#include "Components/TransformComponent.h"
#include "Input/InputListener.h"

void RTSCamera::DefineArchetype(ArchetypeBuilder& builder)
{
	Camera::DefineArchetype(builder);

	builder.AddComponent<CameraScrollComponent>();
	builder.AddComponent<TransformComponent>();
	builder.AddComponent<InputListenerComponent>();
}

void RTSCamera::SetupComponents(ComponentAccessor& accessor)
{
	Camera::SetupComponents(accessor);

	InputListenerComponent* inputListener = accessor.GetComponent<InputListenerComponent>();
	inputListener->m_alwaysActive         = true;
}
