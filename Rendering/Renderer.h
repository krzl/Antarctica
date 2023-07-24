#pragma once

class Entity;
class ComputeShader;
class Settings;

namespace Platform
{
	class Window;
}

namespace Rendering
{
	struct CameraData;
	class RenderQueue;
	class IContext;

	class Renderer
	{
	public:

		static constexpr uint32_t BUFFER_COUNT = 3;

		static Renderer& Get();

		void Init(const Platform::Window& window, const Settings& settings);
		void OnResize(const Platform::Window& window);
		void Render(const RenderQueue& renderQueue, const std::vector<CameraData>& cameras);
		void Cleanup();

		uint32_t GetCurrentBackbufferId() const;

	private:

		IContext* m_context = nullptr;
	};
}
