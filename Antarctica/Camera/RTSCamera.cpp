#include "stdafx.h"
#include "RTSCamera.h"

#include "CameraDragSelectComponent.h"
#include "CameraScrollComponent.h"
#include "Archetypes/ArchetypeBuilder.h"

void RTSCamera::DefineArchetype(ArchetypeBuilder& builder)
{
	Camera::DefineArchetype(builder);

	builder.AddComponent<CameraScrollComponent>();
	builder.AddComponent<CameraDragSelectComponent>();
}
