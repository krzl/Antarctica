#pragma once

#include "Components/Component.h"

namespace Rendering
{
	class DynamicMeshEntity;
}

struct CameraDragSelectComponent : Component
{
	Ref<Rendering::DynamicMeshEntity> m_dragIndicator;

	bool m_isDragging = false;

	Point2DInt m_dragStartPosition;

	DEFINE_CLASS()
};

CREATE_CLASS(CameraDragSelectComponent)
