#include "stdafx.h"
#include "RenderComponent.h"

#include <execution>
#include <Buffers/Types/PerObjectBuffer.h>

#include "Camera.h"

namespace Renderer
{
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

	RenderQueue RenderComponent::GetObjectsToRender(const std::vector<GameObject*>& gameObjects)
	{
		std::atomic_uint32_t counter;

		static RenderQueue renderQueue(1024 * 64);
		renderQueue.resize(1024 * 64);

		const Frustum     cameraFrustum = CameraComponent::Get()->GetFrustum();
		static ThreadPool threadPool    =
			ThreadPool<GameObject>(
				[cameraFrustum, &counter](const GameObject* gameObject)
				{
					for (auto [_, component] : gameObject->GetComponents())
					{
						Ref<RenderComponent> renderComponent = component->GetRef().Cast<RenderComponent>();
						if (renderComponent.IsValid())
						{
							renderComponent->PrepareForRender(renderQueue, cameraFrustum, counter);
						}
					}
				}, 32);

		std::for_each(std::execution::par_unseq, gameObjects.begin(), gameObjects.end(),
					  [cameraFrustum, &counter](const GameObject* gameObject)
					  {
						  for (auto [_, component] : gameObject->GetComponents())
						  {
							  Ref<RenderComponent> renderComponent = component->GetRef().Cast<RenderComponent>();
							  if (renderComponent.IsValid())
							  {
								  renderComponent->PrepareForRender(renderQueue, cameraFrustum, counter);
							  }
						  }
					  });

		renderQueue.resize(counter);
		std::sort(renderQueue.begin(), renderQueue.end(), RenderQueueComp());

		return renderQueue;
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

	void RenderComponent::PrepareForRender(RenderQueue&          renderQueue, const Frustum& cameraFrustum,
										   std::atomic_uint32_t& counter) {}
}
