#pragma once

#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/MeshComponent.h"
#include "Systems/System.h"

namespace Rendering
{
	class Camera;

	class CullingSystem : public System<TransformComponent, MeshComponent, RenderCullComponent, ColliderComponent>
	{
		void OnUpdateStart() override;
		void Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderCullComponent* renderCull, ColliderComponent* collider) override;
		bool IsLockStepSystem() override { return false; }

		Frustum m_cameraFrustum = {};
	};
}
