#pragma once

#include <RenderHandle.h>
#include <Objects/ConstantBuffer.h>

#include "Components/SceneComponent.h"

class RenderComponent : public SceneComponent
{
public:

	static std::set<Renderer::RenderHandle> GetRenderQueue();

	virtual void OnEnabled() override;
	virtual void OnDisabled() override;
	
protected:

	Renderer::ConstantBuffer m_constantBuffer;

	void UpdateConstantBuffer();
	virtual std::vector<Renderer::RenderHandle> PrepareForRender();
};
