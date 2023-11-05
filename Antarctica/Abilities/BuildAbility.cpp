#include "stdafx.h"
#include "BuildAbility.h"

#include "Entities/Camera.h"
#include "Game/BuildingGrid.h"

BuildAbility::BuildAbility(BuildingGrid* buildingGrid, const Point2DInt startPosition, const Point2DInt endPosition):
	m_buildingGrid(buildingGrid),
	m_startPosition(startPosition),
	m_endPosition(endPosition) {}

bool BuildAbility::Init(Entity& entity)
{
	return true;
}

void BuildAbility::Start() {}

bool BuildAbility::Update()
{
	return true;
}

void BuildAbility::End()
{
	m_buildingGrid->SetUsed(m_startPosition, m_endPosition);
}
