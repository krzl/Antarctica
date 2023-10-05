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
		void Update(Entity* entity, TransformComponent* transform, Navigation::MovementComponent* movement) override;
	};
}
