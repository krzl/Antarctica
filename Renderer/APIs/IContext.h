#pragma once
#include <iosfwd>
#include <vector>

class Settings;

namespace Platform
{
	class Window;
}

namespace Renderer
{
	struct SkinningData;
	struct RenderHandle;
	struct CameraData;

	class IContext
	{
	public:

		virtual ~IContext() = default;

		virtual void Init(const Platform::Window& window, const Settings& settings) = 0;
		virtual void FlushCommandQueue() = 0;
		virtual void OnResize(const Platform::Window& window) = 0;

		virtual void WaitForFrameCompletion() const = 0;
		virtual void UpdateSkinning(IComputeShader* computeShader, std::vector<SkinningData>& vector) = 0;
		virtual void SetupCamera(const CameraData& camera) const = 0;
		virtual void SetupRenderTarget(const CameraData& camera) const = 0;
		virtual void DrawObjects(std::priority_queue<RenderHandle>& renderQueue, const CameraData& camera) = 0;
		virtual void FinalizeDrawing() = 0;

		virtual void     ExecuteAndPresent() = 0;
		virtual void     Cleanup() = 0;
		virtual uint32_t GetCurrentBackbufferId() = 0;

		static IContext* CreateContext();
	};
}
