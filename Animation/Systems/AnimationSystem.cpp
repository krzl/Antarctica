#include "stdafx.h"
#include "AnimationSystem.h"

#include "Components/AnimatedMeshComponent.h"

namespace Anim
{
	void AnimationSystem::Update(Entity* entity, AnimatedMeshComponent* animatedMesh)
	{
		animatedMesh->m_animationSolver.UpdateState();
	}
}
