#include "stdafx.h"
#include "ImGuiComponent.h"

#include "Debug/ImGui/ImGuiSystem.h"

BoundingBox ImGuiComponent::GetBoundingBox() const
{
	return BoundingBox(GetWorldPosition() - Vector3D(1.0f, 1.0f, 1.0f),
					   GetWorldPosition() + Vector3D(1.0f, 1.0f, 1.0f));
}

void ImGuiComponent::PrepareForRender(Renderer::RenderQueue& renderQueue, const Frustum& cameraFrustum,
									  std::atomic_uint32_t&  counter)
{
	std::vector<Renderer::QueuedRenderObject>& renderObjects = ImGuiSystem::GetInstance()->Render();

	for (Renderer::QueuedRenderObject& renderObject : renderObjects)
	{
		renderQueue[counter.fetch_add(1)] = &renderObject;
	}
}
