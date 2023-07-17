#pragma once

#include "Entities/Camera.h"

namespace Navigation
{
	class MovementComponent;
}

class MovementComponent;

namespace Rendering
{
	class AnimatedMeshComponent;
}

class Peasant : public GameObject
{
public:

	Peasant();

	void OnCreated() override;

	Rendering::CameraComponent* m_camera; //TODO: REMOVE

	[[nodiscard]] Rendering::AnimatedMeshComponent* GetAnimatedMeshComponent() { return *m_animatedMeshComponent; }
	[[nodiscard]] Navigation::MovementComponent*   GetMovementComponent() { return *m_movementComponent; }

	[[nodiscard]] const Rendering::AnimatedMeshComponent* GetAnimatedMeshComponent() const { return *m_animatedMeshComponent; }
	[[nodiscard]] const Navigation::MovementComponent*   GetMovementComponent() const { return *m_movementComponent; }

protected:

	Ref<Rendering::AnimatedMeshComponent> m_animatedMeshComponent;
	Ref<Navigation::MovementComponent>   m_movementComponent;

	bool m_isWalking = false;

	DEFINE_CLASS()
};

CREATE_CLASS(Peasant)
