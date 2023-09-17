#pragma once

#include "Ability.h"

class BuildingGrid;

class BuildAbility : public Ability
{
public:

	BuildAbility(BuildingGrid* buildingGrid, Point2DInt startPosition, Point2DInt endPosition);

private:

	bool Init(Entity& entity) override;
	void Start() override;
	bool Update() override;
	void End() override;

	BuildingGrid* m_buildingGrid;

	Point2DInt m_startPosition;
	Point2DInt m_endPosition;
};
