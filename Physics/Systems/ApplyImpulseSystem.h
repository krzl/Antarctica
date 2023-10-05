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

	class ApplyImpulseSystem : public System<TransformComponent, Navigation::MovementComponent, PhysicsBodyComponent>
	{
	public:

		ApplyImpulseSystem();

	private:

		void Update(uint64_t entityId, TransformComponent* transform, Navigation::MovementComponent* movement,
					PhysicsBodyComponent* physicsBody) override;
	};
}
