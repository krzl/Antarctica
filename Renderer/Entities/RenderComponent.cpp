#include "stdafx.h"
#include "RenderComponent.h"

#include <Buffers/Types/PerObjectBuffer.h>

namespace Renderer
{
	std::unordered_set<RenderComponent*> renderComponents;

	std::multiset<QueuedRenderObject> RenderComponent::GetObjectsToRender()
	{
		std::multiset<QueuedRenderObject> renderQueue;

		for (RenderComponent* component : renderComponents)
		{
			std::vector<QueuedRenderObject> handles = component->PrepareForRender();
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

	std::vector<uint8_t> RenderComponent::GetConstantBuffer(const uint32_t id)
	{
		const auto worldMatrix = GetWorldTransform() * GetAttachmentTransform(id);

		PerObjectBuffer buffer = PerObjectBuffer::DEFAULT_BUFFER;
		buffer.m_transform     = worldMatrix.transpose;

		std::vector<uint8_t> array(sizeof(buffer));
		memcpy(array.data(), &buffer, sizeof(buffer));
		return array;
	}

	std::vector<QueuedRenderObject> RenderComponent::PrepareForRender()
	{
		return {};
	}
}
