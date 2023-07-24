#include "stdafx.h"
#include "AnimationSystem.h"

#include "Components/AnimatedMeshComponent.h"

namespace Anim
{
	void AnimationSystem::Update(uint64_t entityId, AnimatedMeshComponent* animatedMesh)
	{
		animatedMesh->m_animationSolver.UpdateState();
	}
}
