#pragma once

#include "Components/PhysicsBodyComponent.h"
#include "Systems/System.h"

namespace Navigation
{
	struct MovementComponent;
}
struct TransformComponent;

namespace Physics
{
	struct PhysicsBodyComponent;

	class CollisionGatherSystem : public System<TransformComponent, Navigation::MovementComponent, PhysicsBodyComponent>
	{
		void Update(uint64_t entityId, TransformComponent* transform, Navigation::MovementComponent* movement,
					PhysicsBodyComponent* physicsBody) override;
	};
}
