#pragma once

#include "Components/Component.h"

namespace Rendering
{
	struct CullData
	{
		bool m_isCulled;
		std::bitset<256> m_culledSubmeshes;
	};
	
	struct RenderCullComponent : Component
	{
		friend class CullingSystem;
		friend class RenderSystem;
		friend class SkinningSystem;

		bool m_cullSubmeshes = false;
		bool m_neverCull     = false;

	private:

		std::array<CullData, 16> m_cullData;

		DEFINE_CLASS()
	};

	CREATE_CLASS(RenderCullComponent)
}
