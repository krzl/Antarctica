#pragma once

#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Systems/System.h"

class Mesh;
class ComponentAccessor;

namespace Rendering
{
	struct QueuedRenderObject;

	class RenderQueue : public std::vector<QueuedRenderObject*> {};

	class RenderSystem : public System<TransformComponent, MeshComponent, RenderComponent>
	{
	public:

		[[nodiscard]] const RenderQueue& GetRenderQueue() const { return m_renderQueue; }

	private:

		void OnUpdateStart() override;
		void OnUpdateEnd() override;
		void Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderComponent*) override;

		Transform4D GetAttachedNodeTransform(ComponentAccessor& componentAccessor, const Mesh& mesh, int32_t nodeId, bool ignoreAttachmentRotation);
		Transform4D GetAttachmentTransform(ComponentAccessor& componentAccessor,
			const MeshComponent*                              mesh,
			uint32_t                                          submeshId);

		std::atomic_uint32_t m_counter;
		RenderQueue          m_renderQueue;
		Frustum              m_cameraFrustum = {};
	};
}
