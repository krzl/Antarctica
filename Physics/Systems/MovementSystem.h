#pragma once

#include "Systems/System.h"

namespace Navigation
{
	struct MovementComponent;
}
struct TransformComponent;

namespace Physics
{
	class MovementSystem : public System<TransformComponent, Navigation::MovementComponent>
	{
		void Update(uint64_t entityId, TransformComponent* transform, Navigation::MovementComponent* movement) override;
	};
}
