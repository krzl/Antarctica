#pragma once

#include <RenderObject.h>
#include <Buffers/ConstantBuffer.h>

#include "Components/SceneComponent.h"

namespace Renderer
{
	class RenderComponent : public SceneComponent
	{
	public:

		static std::multiset<QueuedRenderObject> GetObjectsToRender();

		void OnEnabled() override;
		void OnDisabled() override;

	protected:

		virtual Transform4D                     GetAttachmentTransform(uint32_t id);
		std::vector<uint8_t>                    GetConstantBuffer(uint32_t id);
		virtual std::vector<QueuedRenderObject> PrepareForRender();
	};
}
