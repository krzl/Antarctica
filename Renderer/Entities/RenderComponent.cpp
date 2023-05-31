#include "stdafx.h"
#include "RenderComponent.h"

#include <Buffers/Types/PerObjectBuffer.h>

namespace Renderer
{
	std::unordered_set<RenderComponent*> renderComponents;

	std::priority_queue<RenderHandle> RenderComponent::GetRenderQueue()
	{
		std::priority_queue<RenderHandle> renderQueue;

		for (RenderComponent* component : renderComponents)
		{
			std::vector<RenderHandle> handles = component->PrepareForRender();
			for (auto& handle : handles)
			{
				renderQueue.emplace(handle);
			}
		}

		return renderQueue;
	}

	void RenderComponent::OnEnabled()
	{
		renderComponents.insert(this);
	}

	void RenderComponent::OnDisabled()
	{
		renderComponents.erase(this);
	}

	Transform4D RenderComponent::GetAttachmentTransform(uint32_t id)
	{
		return Transform4D::identity;
	}

	void RenderComponent::UpdateConstantBuffer(const uint32_t id)
	{
		const auto       worldMatrix = GetWorldTransform() * GetAttachmentTransform(id);
		PerObjectBuffer* buffer      = m_constantBuffers[id].GetData<PerObjectBuffer>();
		buffer->m_transform          = worldMatrix.transpose;
	}

	std::vector<RenderHandle> RenderComponent::PrepareForRender()
	{
		return {};
	}
}
