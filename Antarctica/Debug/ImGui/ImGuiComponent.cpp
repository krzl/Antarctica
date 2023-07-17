#include "stdafx.h"
#include "ImGuiComponent.h"

#include "Debug/ImGui/ImGuiManager.h"

BoundingBox ImGuiComponent::GetBoundingBox() const
{
	return BoundingBox(GetWorldPosition() - Vector3D(1.0f, 1.0f, 1.0f),
					   GetWorldPosition() + Vector3D(1.0f, 1.0f, 1.0f));
}

void ImGuiComponent::PrepareForRender(Rendering::RenderQueue& renderQueue, const Frustum& cameraFrustum,
									  std::atomic_uint32_t&  counter)
{
	std::vector<Rendering::QueuedRenderObject>& renderObjects = ImGuiManager::GetInstance()->Render();

	for (Rendering::QueuedRenderObject& renderObject : renderObjects)
	{
		renderQueue[counter.fetch_add(1)] = &renderObject;
	}
}
