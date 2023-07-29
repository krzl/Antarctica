#pragma once

#include "Components/ColliderComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Systems/System.h"

struct TransformComponent;

class Application;

namespace Rendering
{
	struct MeshComponent;
	class Camera;

	class CullingSystem : public System<TransformComponent, MeshComponent, RenderCullComponent, ColliderComponent, RenderComponent>
	{
		friend class Application;

		void OnUpdateStart() override;
		void Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderCullComponent* renderCull,
					ColliderComponent* collider, RenderComponent* render) override;

		Frustum m_cameraFrustum = {};
	};
}
