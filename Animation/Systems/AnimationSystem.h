#pragma once

#include "Systems/System.h"

namespace Anim
{
	struct AnimatedMeshComponent;

	class AnimationSystem : public System<AnimatedMeshComponent>
	{
		void Update(Entity* entity, AnimatedMeshComponent* animatedMesh) override;
	};
}
