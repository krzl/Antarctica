#pragma once

#include <RenderHandle.h>
#include <Objects/ConstantBuffer.h>

#include "Components/SceneComponent.h"

namespace Renderer
{
	class RenderComponent : public SceneComponent
	{
	public:

		static std::priority_queue<Renderer::RenderHandle> GetRenderQueue();

		void OnEnabled() override;
		void OnDisabled() override;
		void Tick() override;

	protected:

		Renderer::ConstantBuffer m_constantBuffer;

		void                                        UpdateConstantBuffer();
		virtual std::vector<Renderer::RenderHandle> PrepareForRender();
	};
}
