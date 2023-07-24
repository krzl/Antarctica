#pragma once

#include "System.h"
#include "Components/ColliderComponent.h"
#include "Components/MoveableComponent.h"
#include "Components/TransformComponent.h"

class QuadtreeUpdateSystem : public System<TransformComponent, MoveableComponent, ColliderComponent>
{
	void OnUpdateEnd() override;
	void Update(uint64_t entityId, TransformComponent* transform, MoveableComponent* moveable, ColliderComponent* collider) override;
	bool IsLockStepSystem() override { return true; }
};
