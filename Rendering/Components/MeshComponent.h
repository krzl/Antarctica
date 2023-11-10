#pragma once

#include "Components/Component.h"

class Material;
class Mesh;

namespace Rendering
{
	struct QueuedRenderObject;

	struct RenderItem
	{
		std::shared_ptr<Mesh> m_mesh;
		std::vector<std::shared_ptr<Material>> m_materials;
		std::vector<std::optional<Rect>> m_rectMasks;

		bool m_isAnimated = false;

		bool m_isHidden = false;

		Transform4D m_transform = Transform4D::identity;
	};

	struct MeshComponent : Component
	{
		std::vector<RenderItem> m_renderItems;

		DEFINE_CLASS()
	};

	CREATE_CLASS(MeshComponent)
}
