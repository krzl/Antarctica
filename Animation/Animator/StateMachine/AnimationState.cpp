#include "stdafx.h"
#include "AnimationState.h"

#include "Solver/Solver.h"

namespace Anim
{
	MeshBoneTransforms AnimationState::CalculateBones(
		const std::vector<const Skeleton*> skeletons,
		const float                        currentTime) const
	{
		const float animationTime = m_isLooping ? fmod(currentTime, m_animation->m_duration) : currentTime;
		return Solver::Calculate(m_animation, skeletons, animationTime);
	}
}
