#include "stdafx.h"
#include "Renderer.h"

#include "APIs/IContext.h"
#include "Entities/CameraComponent.h"
#include "Entities/RenderComponent.h"
#include "Time/Timer.h"

namespace Rendering
{
	struct SkinningObjectData;
	static Renderer* renderer;

	Renderer& Renderer::Get()
	{
		return *renderer;
	}

	void Renderer::Init(const Platform::Window& window, const Settings& settings)
	{
		renderer = this;

		m_context = IContext::CreateContext();
		m_context->Init(window, settings);

		OnResize(window);
	}

	void Renderer::OnResize(const Platform::Window& window)
	{
		m_context->OnResize(window);
	}

	void Renderer::Render(const std::vector<GameObject*>& gameObjects)
	{
		RenderQueue                     objectsToRender = RenderComponent::GetObjectsToRender(gameObjects);
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

	void Renderer::Cleanup()
	{
		m_context->Cleanup();
	}

	uint32_t Renderer::GetCurrentBackbufferId() const
	{
		return m_context->GetCurrentBackbufferId();
	}
}
