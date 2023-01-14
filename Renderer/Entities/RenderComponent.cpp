#include "stdafx.h"
#include "RenderComponent.h"

#include <Buffers/PerObjectBuffer.h>

namespace Renderer
{
	std::unordered_set<RenderComponent*> renderComponents;

	std::priority_queue<Renderer::RenderHandle> RenderComponent::GetRenderQueue()
	{
		std::priority_queue<Renderer::RenderHandle> renderQueue;

		for (RenderComponent* component : renderComponents)
		{
			std::vector<Renderer::RenderHandle> handles = component->PrepareForRender();
			for (auto& handle : handles)
			{
				renderQueue.emplace(handle);
			}
		}

		return renderQueue;
	}

	void RenderComponent::OnEnabled()
	{
		renderComponents.insert(*GetRef().Cast<RenderComponent>());
		if (!m_constantBuffer.IsInitialized())
		{
			m_constantBuffer.Init(1, sizeof(Renderer::PerObjectBuffer), &Renderer::PerObjectBuffer::DEFAULT_BUFFER);
		}
	}

	void RenderComponent::OnDisabled()
	{
		renderComponents.erase(*GetRef().Cast<RenderComponent>());
	}

	void RenderComponent::Tick()
	{
		SetLocalRotation(m_rotation * Quaternion::MakeRotationY(0.00087266462f));
	}

	void RenderComponent::UpdateConstantBuffer()
	{
		const auto                 worldMatrix = GetWorldTransform();
		Renderer::PerObjectBuffer* buffer      = m_constantBuffer.GetData<Renderer::PerObjectBuffer>();
		buffer->m_transform                    = worldMatrix.transpose;
	}

	std::vector<Renderer::RenderHandle> RenderComponent::PrepareForRender()
	{
		return {};
	}
}
