#pragma once

#include "Entities/RenderComponent.h"

class ImGuiComponent : public Rendering::RenderComponent
{
public:

	BoundingBox GetBoundingBox() const override;

protected:

	void PrepareForRender(Rendering::RenderQueue& renderQueue, const Frustum& cameraFrustum,
						  std::atomic_uint32_t&  counter) override;
	DEFINE_CLASS()
};

CREATE_CLASS(ImGuiComponent)
