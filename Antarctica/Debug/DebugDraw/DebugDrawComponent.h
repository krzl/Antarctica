#pragma once

#include "Assets/DynamicSubmesh.h"
#include "Debug/DebugDrawSystem.h"
#include "Entities/RenderComponent.h"

class DebugDrawComponent : public Renderer::RenderComponent
{
	struct DebugDrawElement
	{
		std::vector<uint8_t>      m_vertexData;
		std::vector<uint32_t>     m_indices;
		float                     m_despawnTime = 0.0f;
		BoundingBox               m_boundingBox;
		std::shared_ptr<Material> m_material;

		DynamicSubmesh m_submesh;

		std::unique_ptr<Renderer::QueuedRenderObject> m_cachedRenderObject;

		DebugDrawElement() = default;

		explicit DebugDrawElement(DebugDrawSystem::ElementBuilder&& builder);
	};

public:

	BoundingBox GetBoundingBox() const override;

protected:

	void OnCreated() override;

	void PrepareForRender(Renderer::RenderQueue& renderQueue, const Frustum& cameraFrustum,
						  std::atomic_uint32_t&  counter) override;

private:

	std::vector<std::unique_ptr<DebugDrawElement>> m_drawElements;

	DEFINE_CLASS()
};

CREATE_CLASS(DebugDrawComponent)
