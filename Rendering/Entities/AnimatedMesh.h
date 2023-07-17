#pragma once

#include "AnimatedMeshComponent.h"
#include "GameObjects/GameObject.h"

namespace Rendering
{
	class AnimatedMesh : public GameObject
	{
	public:

		AnimatedMesh();

		Ref<AnimatedMeshComponent> GetAnimatedMeshComponent() const
		{
			return m_animatedMeshComponent;
		}

	protected:

		Ref<AnimatedMeshComponent> m_animatedMeshComponent;

		DEFINE_CLASS()
	};

	CREATE_CLASS(AnimatedMesh)
}
