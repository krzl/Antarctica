#pragma once

#include "State.h"
#include "Assets/Animation.h"

namespace Anim
{
	class AnimationState : public State
	{
	public:

		explicit AnimationState(std::shared_ptr<Animation> animation)
			: m_animation(std::move(animation)) {}

		std::vector<Transform4D> CalculateBones(
			const std::vector<MeshNode>& meshNodes, const float currentTime) const override;

		bool m_isLooping = true;
		
	private:

		std::shared_ptr<Animation> m_animation;
	};
}
