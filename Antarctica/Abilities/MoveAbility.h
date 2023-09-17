#pragma once

#include "Ability.h"

class MoveAbility : public Ability
{
	bool Init(Entity& entity) override;
	void Start() override;
	bool Update() override;
	void End() override;

	Point3D m_target = Point3D();
};
