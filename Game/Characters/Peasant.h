#pragma once

#include "Entities/Camera.h"

namespace Navigation
{
	class MovementComponent;
}

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

	Renderer::CameraComponent* m_camera; //TODO: REMOVE

	[[nodiscard]] Renderer::AnimatedMeshComponent* GetAnimatedMeshComponent() { return *m_animatedMeshComponent; }
	[[nodiscard]] Navigation::MovementComponent*   GetMovementComponent() { return *m_movementComponent; }

	[[nodiscard]] const Renderer::AnimatedMeshComponent* GetAnimatedMeshComponent() const { return *m_animatedMeshComponent; }
	[[nodiscard]] const Navigation::MovementComponent*   GetMovementComponent() const { return *m_movementComponent; }

protected:

	Ref<Renderer::AnimatedMeshComponent> m_animatedMeshComponent;
	Ref<Navigation::MovementComponent>   m_movementComponent;

	bool m_isWalking = false;

	DEFINE_CLASS()
};

CREATE_CLASS(Peasant)
