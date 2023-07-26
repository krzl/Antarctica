#pragma once

#include "State.h"
#include "Assets/Animation.h"

namespace Anim
{
	class AnimationState : public State
	{
	public:

		explicit AnimationState(std::shared_ptr<Animation> animation) :
			m_animation(std::move(animation)) {}

		void CalculateBones(std::vector<Transform4D>& boneMatrices, const std::vector<MeshNode>& meshNodes, const float currentTime) const override;

	private:

		std::shared_ptr<Animation> m_animation;

		bool m_isLooping = true;
	};
}
