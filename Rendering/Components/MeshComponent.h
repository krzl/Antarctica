#pragma once

#include "Components/Component.h"

class Material;
class Mesh;

namespace Rendering
{
	struct QueuedRenderObject;

	struct MeshComponent : Component
	{
		std::shared_ptr<Mesh> m_mesh;
		std::vector<std::shared_ptr<Material>> m_materials;
		std::vector<std::optional<Rect>> m_rectMasks;

		Transform4D m_transform = Transform4D::identity;
		
		bool m_useMeshForCollision = false;

		DEFINE_CLASS()
	};

	CREATE_CLASS(MeshComponent)
}
