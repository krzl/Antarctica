#pragma once

#include <RenderObject.h>

#include "Buffers/Types/PerObjectBuffer.h"
#include "Components/SceneComponent.h"

namespace Renderer
{
	typedef std::vector<QueuedRenderObject> RenderQueue;
	
	class RenderComponent : public SceneComponent
	{
	public:

		static RenderQueue GetObjectsToRender();

		void OnEnabled() override;
		void OnDisabled() override;

	protected:

		virtual Transform4D GetAttachmentTransform(uint32_t id);
		PerObjectBuffer     GetConstantBuffer(uint32_t id);
		virtual void        PrepareForRender(RenderQueue& renderQueue);
	};
}
