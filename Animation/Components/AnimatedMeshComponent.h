#pragma once

#include "Components/Component.h"
#include "Solver/Solver.h"

namespace Anim
{
	struct AnimatedMeshComponent : Component
	{
		std::shared_ptr<Animator> m_animator;
		std::vector<Transform4D> m_animatedTransforms;
		Solver m_animationSolver;

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMeshComponent)
}
