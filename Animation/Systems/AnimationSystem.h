#pragma once

#include "Systems/System.h"

namespace Anim
{
	struct AnimatedMeshComponent;

	class AnimationSystem : public System<AnimatedMeshComponent>
	{
		void Update(uint64_t entityId, AnimatedMeshComponent* animatedMesh) override;
	};
}
