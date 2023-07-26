#pragma once

#include "Components/ColliderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Systems/System.h"

struct TransformComponent;

class Application;

namespace Rendering
{
	struct MeshComponent;
	class Camera;

	class CullingSystem : public System<TransformComponent, MeshComponent, RenderCullComponent, ColliderComponent>
	{
		friend class Application;
		
		void OnUpdateStart() override;
		void Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderCullComponent* renderCull, ColliderComponent* collider) override;

		Frustum m_cameraFrustum = {};

		FrameCounter* m_frameCounter;
	};
}
