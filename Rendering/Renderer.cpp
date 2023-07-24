#include "stdafx.h"
#include "Renderer.h"

#include "CameraData.h"
#include "APIs/IContext.h"

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

	void Renderer::Render(const RenderQueue& renderQueue, const std::vector<CameraData>& cameras)
	{
		m_context->WaitForFrameCompletion();

		m_context->CreateRenderQueue(renderQueue);
		m_context->UpdateSkinning();

		for (uint32_t i = 0; i < cameras.size(); ++i)
		{
			const CameraData& camera = cameras[i];

			m_context->SetupCamera(camera);
			m_context->SetupRenderTarget(camera);
			m_context->DrawObjects(camera);
			m_context->FinalizeDrawing();
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
