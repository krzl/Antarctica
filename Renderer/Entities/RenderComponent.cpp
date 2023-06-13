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

		constexpr uint32_t numThreads    = 8;
		const uint32_t     partitionSize = (uint32_t) renderComponents.size() / numThreads;

		std::vector<std::unordered_set<RenderComponent*>::iterator> iterators(numThreads + 1);
		auto                                                        start = iterators[0] = renderComponents.begin();
		for (uint32_t i = 0; i < numThreads - 1; ++i)
		{
			const auto end = std::next(start, partitionSize);

			start = iterators[i + 1] = end;
		}
		iterators[numThreads] = renderComponents.end();

		std::vector<std::thread> threads;
		if (threads.size() == 0)
		{
			for (uint32_t i = 0; i < numThreads; ++i)
			{
				threads.emplace_back([i, iterators, renderQueuePtr]
				{
					std::for_each(std::execution::par, iterators[i], iterators[i + 1], [this, renderQueuePtr](RenderComponent* component)
					{
						component->PrepareForRender(*renderQueuePtr);
					});
				});
			}
		}

		for (auto& thread : threads)
		{
			thread.join();
		}


		std::sort(renderQueue.begin(), renderQueue.end(), RenderQueueComp());

		capacity = max(capacity, (uint32_t) renderQueue.capacity());

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
