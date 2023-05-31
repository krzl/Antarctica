#pragma once

#include "Entities/Camera.h"

class MovementComponent;

namespace Renderer
{
	class AnimatedMeshComponent;
}

class Peasant : public GameObject
{
public:

	Peasant();

protected:

	Ref<Renderer::AnimatedMeshComponent> m_animatedMeshComponent;
	Ref<MovementComponent>               m_movementComponent;

	DEFINE_CLASS()
};


CREATE_CLASS(Peasant)
