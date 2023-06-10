#include "stdafx.h"
#include "RenderComponent.h"

#include <execution>
#include <Buffers/Types/PerObjectBuffer.h>

namespace Renderer
{
	std::unordered_set<RenderComponent*> renderComponents;

	std::mutex RenderComponent::renderQueueMutex;

	struct RenderQueueComp
	{
		bool operator()(const QueuedRenderObject* a, const QueuedRenderObject* b) const
		{
			if (a == nullptr)
				return a;
			if (b == nullptr)
				return b;
			return (*a < *b);
		}
	};

	RenderQueue RenderComponent::GetObjectsToRender()
	{
		static uint32_t capacity = 10000;

		RenderQueue renderQueue;
		renderQueue.reserve(capacity);

		RenderQueue* renderQueuePtr = &renderQueue;

		constexpr uint32_t numThreads = 8;
		const uint32_t     partitionSize = renderComponents.size() / numThreads;

		std::vector<std::thread> threads;

		auto start = renderComponents.begin();
		for (uint32_t i = 0; i < numThreads - 1; ++i)
		{
			auto end = std::next(start, partitionSize);
			threads.emplace_back([start, end, renderQueuePtr]
			{
				std::for_each(std::execution::par, start, end, [this, renderQueuePtr](RenderComponent* component)
				{
					component->PrepareForRender(*renderQueuePtr);
				});
			});
			start = end;
		}

		if (start != renderComponents.end())
		{
			threads.emplace_back([start, renderQueuePtr]
			{
				std::for_each(std::execution::par, start, renderComponents.end(),
							  [this, renderQueuePtr](RenderComponent* component)
							  {
								  component->PrepareForRender(*renderQueuePtr);
							  });
			});
		}

		for (auto& thread : threads)
		{
			thread.join();
		}

		std::sort(renderQueue.begin(), renderQueue.end(), RenderQueueComp());

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
