#pragma once
#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>

#include "RenderObject.h"
#include "Entities/RenderComponent.h"

class ComputeShader;
class Settings;

namespace Platform
{
	class Window;
}

namespace Rendering
{
	struct SkinningObjectData;
	struct QueuedRenderObject;
	struct CameraData;

	class IContext
	{
	public:

		virtual ~IContext() = default;

		virtual void Init(const Platform::Window& window, const Settings& settings) = 0;
		virtual void FlushCommandQueue() = 0;
		virtual void OnResize(const Platform::Window& window) = 0;

		virtual void WaitForFrameCompletion() = 0;

		virtual void CreateRenderQueue(RenderQueue& objectsToRender) = 0;

		virtual void UpdateSkinning() = 0;
		virtual void SetupCamera(const CameraData& camera) const = 0;
		virtual void SetupRenderTarget(const CameraData& camera) const = 0;
		virtual void DrawObjects(const CameraData& camera) = 0;
		virtual void FinalizeDrawing() = 0;

		virtual void     ExecuteAndPresent() = 0;
		virtual void     Cleanup() = 0;
		virtual uint32_t GetCurrentBackbufferId() = 0;

		static IContext* CreateContext();

	protected:

		std::shared_ptr<ComputeShader> m_skinningShader;
	};
}
