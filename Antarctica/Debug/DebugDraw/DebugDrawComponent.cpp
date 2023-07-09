#include "stdafx.h"
#include "DebugDrawComponent.h"

#include "Debug/DebugDrawSystem.h"

BoundingBox DebugDrawComponent::GetBoundingBox() const
{
	return BoundingBox(GetWorldPosition() - Vector3D(1.0f, 1.0f, 1.0f),
					   GetWorldPosition() + Vector3D(1.0f, 1.0f, 1.0f));
}

void DebugDrawComponent::PrepareForRender(Renderer::RenderQueue& renderQueue, const Frustum& cameraFrustum,
										  std::atomic_uint32_t&  counter)
{
	DebugDrawSystem::GetInstance()->Render(renderQueue, cameraFrustum, counter);
}
