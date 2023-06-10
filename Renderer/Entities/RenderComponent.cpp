#include "stdafx.h"
#include "RenderComponent.h"

#include <Buffers/Types/PerObjectBuffer.h>

namespace Renderer
{
	std::unordered_set<RenderComponent*> renderComponents;

	RenderQueue RenderComponent::GetObjectsToRender()
	{
		static uint32_t capacity = 10000;
		
		RenderQueue renderQueue;
		renderQueue.reserve(capacity);

		for (RenderComponent* component : renderComponents)
		{
			component->PrepareForRender(renderQueue);
		}

		std::sort(renderQueue.begin(), renderQueue.end());

		capacity = max(capacity, renderQueue.capacity());
		
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

	PerObjectBuffer RenderComponent::GetConstantBuffer(const uint32_t id)
	{
		const Transform4D worldMatrix = GetWorldTransform() * GetAttachmentTransform(id);

		PerObjectBuffer buffer = PerObjectBuffer::DEFAULT_BUFFER;
		buffer.m_transform     = worldMatrix.transpose;

		return buffer;
	}

	void RenderComponent::PrepareForRender(RenderQueue& renderQueue) {}
}
