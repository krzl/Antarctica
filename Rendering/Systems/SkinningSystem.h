#pragma once

#include "Components/AnimatedMeshComponent.h"
#include "Systems/System.h"

namespace Rendering
{
	struct RenderComponent;
	struct MeshComponent;

	class SkinningSystem : public System<Anim::AnimatedMeshComponent, MeshComponent, RenderComponent>
	{
		void Update(uint64_t entityId, Anim::AnimatedMeshComponent* animatedMesh, MeshComponent* mesh, RenderComponent* render) override;
		bool IsLockStepSystem() override { return false; }
	};
}
