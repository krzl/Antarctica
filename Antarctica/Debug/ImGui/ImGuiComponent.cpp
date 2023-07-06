#include "stdafx.h"
#include "ImGuiComponent.h"

#include "ImGuiSystem.h"

ImGuiComponent::ImGuiComponent()
{
	SetLocalPosition(Point3D(0.0f, 0.0f, -1.0f)); //in order to make it in front of near plane, so it won't get frustum culled
	m_isTickable = true;
}

BoundingBox ImGuiComponent::GetBoundingBox() const
{
	return RenderComponent::GetBoundingBox();
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
