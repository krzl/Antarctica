#pragma once

#include "Ability.h"

class MoveAbility : public Ability
{
	bool Init(Entity& entity) override;
	void Start(Entity& entity) override;
	bool Update(Entity& entity) override;
	void End(Entity& entity) override;

	Point3D m_target = Point3D();

	DEFINE_CLASS()
};

CREATE_CLASS(MoveAbility)
