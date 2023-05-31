#include "stdafx.h"
#include "RenderSystem.h"

#include "AssetManager.h"
#include "APIs/IContext.h"
#include "APIs/Dx12/Shaders/ComputeShader.h"
#include "Assets/ComputeShader.h"
#include "Entities/AnimatedMeshComponent.h"
#include "Entities/CameraComponent.h"
#include "Entities/RenderComponent.h"

namespace Renderer
{
	struct SkinningData;
	static RenderSystem* renderSystem;

	RenderSystem& RenderSystem::Get()
	{
		return *renderSystem;
	}

	void RenderSystem::Init(const Platform::Window& window, const Settings& settings)
	{
		renderSystem = this;

		m_context = IContext::CreateContext();
		m_context->Init(window, settings);

		OnResize(window);
	}

	void RenderSystem::OnResize(const Platform::Window& window)
	{
		m_context->OnResize(window);
	}

	void RenderSystem::Render()
	{
		std::vector<SkinningData>         skinningData = AnimatedMeshComponent::GetAllSkinningData();
		std::priority_queue<RenderHandle> renderQueue  = RenderComponent::GetRenderQueue();
		std::priority_queue<CameraData>   cameras      = CameraComponent::GetAllCameraData();

		m_context->WaitForFrameCompletion();
		
		if (skinningData.size() > 0)
		{
			if (m_skinningShader == nullptr)
			{
				m_skinningShader = AssetManager::GetAsset<ComputeShader>("../Resources/Shaders/Compute/skinning.hlsl");
				m_skinningShader->SetNativeObject(IComputeShader::Create(m_skinningShader));
			}
			m_context->UpdateSkinning(m_skinningShader->GetNativeObject(), skinningData);
		}
		
		while (!cameras.empty())
		{
			const CameraData& camera = cameras.top();

			m_context->SetupCamera(camera);
			m_context->SetupRenderTarget(camera);
			m_context->DrawObjects(renderQueue, camera);
			m_context->FinalizeDrawing();

			cameras.pop();
		}

		m_context->ExecuteAndPresent();
	}

	void RenderSystem::Cleanup()
	{
		m_context->Cleanup();
	}

	uint32_t RenderSystem::GetCurrentBackbufferId() const
	{
		return m_context->GetCurrentBackbufferId();
	}
}
