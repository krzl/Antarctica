#pragma once

#include <RenderHandle.h>
#include <Buffers/ConstantBuffer.h>

#include "Components/SceneComponent.h"

namespace Renderer
{
	class RenderComponent : public SceneComponent
	{
	public:

		static std::priority_queue<RenderHandle> GetRenderQueue();

		void OnEnabled() override;
		void OnDisabled() override;

	protected:

		ConstantBuffer m_constantBuffer;

		void                              UpdateConstantBuffer();
		virtual std::vector<RenderHandle> PrepareForRender();
	};
}
