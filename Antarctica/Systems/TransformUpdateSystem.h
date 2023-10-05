#pragma once

#include "Components/MoveableComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"

#include "Systems/System.h"

class TransformUpdateSystem : public System<TransformComponent, MoveableComponent, Rendering::RenderComponent>
{
	void OnUpdateStart() override;
	void Update(Entity* entity, TransformComponent* transform, MoveableComponent* moveable, Rendering::RenderComponent* render) override;

	float m_stepLockProgress = 0;
};
