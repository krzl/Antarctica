#pragma once

#include "RenderObject.h"
#include "Components/Component.h"

class Material;
class Mesh;

namespace Rendering
{
	struct RenderComponent : Component
	{
		std::vector<QueuedRenderObject> m_renderHandles;

		DEFINE_CLASS()
	};

	CREATE_CLASS(RenderComponent)
}
