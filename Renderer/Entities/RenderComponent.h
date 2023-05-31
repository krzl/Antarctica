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

		std::vector<ConstantBuffer> m_constantBuffers;

		virtual Transform4D               GetAttachmentTransform(uint32_t id);
		void                              UpdateConstantBuffer(uint32_t id);
		virtual std::vector<RenderHandle> PrepareForRender();
	};
}
