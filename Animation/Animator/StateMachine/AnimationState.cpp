#include "stdafx.h"
#include "AnimationState.h"

#include "Solver/Solver.h"

namespace Anim
{
	void AnimationState::CalculateBones(std::vector<Transform4D>& boneMatrices, const std::vector<MeshNode>& meshNodes,
										const float               currentTime) const
	{
		const float animationTime = m_isLooping ? fmod(currentTime, m_animation->m_duration) : currentTime;
		return Solver::Calculate(boneMatrices, m_animation, meshNodes, animationTime);
	}
}
