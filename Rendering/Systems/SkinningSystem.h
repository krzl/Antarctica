#pragma once

#include "Components/AnimatedMeshComponent.h"
#include "Systems/System.h"

namespace Rendering
{
	struct RenderComponent;
	struct MeshComponent;

	class SkinningSystem : public System<Anim::AnimatedMeshComponent, MeshComponent, RenderComponent>
	{
		void Update(Entity* entity, Anim::AnimatedMeshComponent* animatedMesh, MeshComponent* mesh, RenderComponent* render) override;
	};
}
