#include "stdafx.h"
#include "AnimatorBuilder.h"

namespace Anim
{
	AnimatorBuilder::AnimatorBuilder(const std::shared_ptr<StateMachine> mainLayer)
	{
		AddLayer(mainLayer);
	}

	void AnimatorBuilder::AddLayer(std::shared_ptr<StateMachine> layer)
	{
		if (m_layers.size() > 0)
		{
			throw "TODO: Only single layer animators are supported";
		}
		m_layers.emplace_back(layer);
	}

	std::shared_ptr<Animator> AnimatorBuilder::Build()
	{
		return std::make_shared<Animator>(std::move(m_layers));
	}
}
