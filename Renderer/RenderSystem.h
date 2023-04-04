#pragma once

class ComputeShader;
class Settings;

namespace Platform
{
	class Window;
}

namespace Renderer
{
	class IContext;

	class RenderSystem
	{
	public:

		static constexpr uint32_t BUFFER_COUNT = 3;

		static RenderSystem& Get();

		void Init(const Platform::Window& window, const Settings& settings);
		void OnResize(const Platform::Window& window);
		void Render();
		void Cleanup();

		uint32_t GetCurrentBackbufferId() const;

	private:

		IContext* m_context = nullptr;

		std::shared_ptr<ComputeShader> m_skinningShader;
	};
}
