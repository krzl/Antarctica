#pragma once

#include "Components/Component.h"

namespace Rendering
{
	struct RenderCullComponent : Component
	{
		friend class CullingSystem;
		friend class RenderSystem;
		friend class SkinningSystem;

		bool m_cullSubmeshes = false;

	private:

		bool              m_isCulled;
		std::bitset<1024> m_culledSubmeshes;


		DEFINE_CLASS()
	};

	CREATE_CLASS(RenderCullComponent)
}
