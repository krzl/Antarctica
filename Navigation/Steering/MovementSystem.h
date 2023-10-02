#pragma once

#include "Systems/System.h"

struct TransformComponent;

namespace Navigation
{
	struct MovementComponent;

	class MovementSystem : public System<TransformComponent, MovementComponent>
	{
		void Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement) override;

		
	};
}
