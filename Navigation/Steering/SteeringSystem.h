#pragma once

#include "Components/MovementComponent.h"
#include "Debug/MovementTester.h"
#include "Systems/System.h"

namespace Navigation
{
	struct NearbyTarget
	{
		const TransformComponent* m_transform = nullptr;
		const MovementComponent*  m_movement  = nullptr;
	};

	class SteeringSystem : public System<TransformComponent, MovementComponent>
	{
		void OnUpdateStart() override;
		void Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement) override;
		bool IsLockStepSystem() override { return true; }

		MovementTester m_movementTester;
	};
}
