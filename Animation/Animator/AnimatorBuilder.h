#pragma once

#include "Animator.h"

namespace Anim
{
	class AnimatorBuilder
	{
	public:

		explicit AnimatorBuilder(std::shared_ptr<StateMachine> mainLayer);

		void AddLayer(std::shared_ptr<StateMachine> layer);

		std::shared_ptr<Animator> Build();

	private:

		std::vector<std::shared_ptr<StateMachine>> m_layers;
	};
}
