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

	void OnCreated() override;

protected:

	Ref<Renderer::AnimatedMeshComponent> m_animatedMeshComponent;
	Ref<MovementComponent>               m_movementComponent;

	bool m_isWalking = false;
	DispatchHandle<>  m_handle;

	DEFINE_CLASS()
};


CREATE_CLASS(Peasant)
