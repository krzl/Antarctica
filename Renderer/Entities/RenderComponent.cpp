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
		if (!m_constantBuffer.IsInitialized())
		{
			m_constantBuffer.Init(1, sizeof(PerObjectBuffer), &PerObjectBuffer::DEFAULT_BUFFER);
		}
	}

	void RenderComponent::OnDisabled()
	{
		renderComponents.erase(this);
	}

	void RenderComponent::UpdateConstantBuffer()
	{
		const auto       worldMatrix = GetWorldTransform();
		PerObjectBuffer* buffer      = m_constantBuffer.GetData<PerObjectBuffer>();
		buffer->m_transform          = worldMatrix.transpose;
	}

	std::vector<RenderHandle> RenderComponent::PrepareForRender()
	{
		return {};
	}
}
