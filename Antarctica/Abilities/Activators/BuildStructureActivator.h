#pragma once

#include "AbilityActivator.h"

#include "Core/Application.h"

#include "Game/BuildingGrid.h"

namespace Rendering
{
	class DynamicMeshEntity;
}

class BuildStructureActivator : public AbilityActivator
{
public:

	BuildStructureActivator(uint32_t width, uint32_t height);

	void Update() override;

	bool CanBeFinished() override;
	bool ShouldBeCancelled() override;

	std::shared_ptr<Ability> Activate(Entity* entity) override;
	void OnFinished() override;

	[[nodiscard]] uint32_t GetWidth() const { return m_width; }
	[[nodiscard]] uint32_t GetHeight() const { return m_height; }

private:

	uint32_t m_width  = 0;
	uint32_t m_height = 0;

	Ref<Rendering::DynamicMeshEntity> m_placementIndicator;

	BuildingGrid* m_buildingGrid = nullptr;

	Point2DInt m_startGridPosition;
	Point2DInt m_endGridPosition;

	bool m_isGridPositionSet = false;
	bool m_canBePlaced       = false;
	bool m_isValid           = false;
};
