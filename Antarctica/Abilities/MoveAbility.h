#pragma once

#include "Ability.h"

class MoveAbility : public Ability
{
public:

	explicit MoveAbility(const Point3D& target, uint32_t delay);

private:

	bool Init(Entity& entity) override;
	void Start() override;
	bool Update() override;
	void End() override;

	Point3D m_target = Point3D();
	uint32_t m_delay = 0;
};
