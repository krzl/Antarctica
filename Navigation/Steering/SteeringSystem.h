#pragma once

#include "Debug/MovementTester.h"
#include "Systems/System.h"

namespace Navigation
{
	struct NearbyTarget
	{
		const TransformComponent* m_transform = nullptr;
		const MovementComponent* m_movement   = nullptr;
	};

	class SteeringSystem : public System<TransformComponent, MovementComponent>
	{
		friend class Application;

		void DrawMovementTester();
		void OnStepLockStart() override;
		void OnStepLockEnd() override;
		void Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement) override;

		MovementTester m_movementTester;
	};
}