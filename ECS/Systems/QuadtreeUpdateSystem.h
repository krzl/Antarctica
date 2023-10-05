#pragma once

#include "System.h"
#include "Components/ColliderComponent.h"
#include "Components/MoveableComponent.h"

struct TransformComponent;

class QuadtreeUpdateSystem : public System<TransformComponent, MoveableComponent, ColliderComponent>
{
	void OnUpdateEnd() override;
	void Update(Entity* entity, TransformComponent* transform, MoveableComponent* moveable, ColliderComponent* collider) override;
};
