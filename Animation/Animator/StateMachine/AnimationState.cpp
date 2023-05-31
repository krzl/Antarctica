#include "stdafx.h"
#include "AnimationState.h"

#include "Solver/Solver.h"

namespace Anim
{
	std::vector<Transform4D> AnimationState::CalculateBones(const std::vector<MeshNode>& meshNodes,
															const float                  currentTime) const
	{
		const float animationTime = m_isLooping ? fmod(currentTime, m_animation->m_duration) : currentTime;
		return Solver::Calculate(m_animation, meshNodes, animationTime);
	}
}
