#include "stdafx.h"
#include "RenderSystem.h"

#include "APIs/IContext.h"
#include "Entities/CameraComponent.h"
#include "Entities/RenderComponent.h"
#include "Time/Timer.h"

namespace Renderer
{
	struct SkinningObjectData;
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
		RenderQueue                     objectsToRender = RenderComponent::GetObjectsToRender();
		std::priority_queue<CameraData> cameras         = CameraComponent::GetAllCameraData();
		
		m_context->WaitForFrameCompletion();

		m_context->CreateRenderQueue(objectsToRender);
		m_context->UpdateSkinning();

		while (!cameras.empty())
		{
			const CameraData& camera = cameras.top();

			m_context->SetupCamera(camera);
			m_context->SetupRenderTarget(camera);
			m_context->DrawObjects(camera);
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
