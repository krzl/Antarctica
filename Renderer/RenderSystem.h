#pragma once
#include <functional>
#include <set>


#include "CameraData.h"
#include "RenderHandle.h"

class Settings;

namespace Platform
{
	class Window;
}

namespace Renderer
{
	class RenderSystem
	{
	public:

		static constexpr uint32_t BUFFER_COUNT = 3;

		static RenderSystem& Get();

		void Init(const Platform::Window& window, const Settings& settings);
		void FlushCommandQueue();
		void OnResize(const Platform::Window& window);
		void Render(std::priority_queue<RenderHandle>& renderQueue, std::priority_queue<CameraData>& cameras);
		void Present();
		void Cleanup();

		[[nodiscard]] const ComPtr<IDXGIFactory4>& GetDXGIFactory() const
		{
			return m_dxgiFactory;
		}

		[[nodiscard]] const ComPtr<ID3D12Device>& GetDevice() const
		{
			return m_device;
		}

		[[nodiscard]] const ComPtr<ID3D12GraphicsCommandList>& GetCommandList() const
		{
			return m_commandList;
		}

		[[nodiscard]] uint32_t GetMsNumQualityLevels() const
		{
			return m_msNumQualityLevels;
		}

		[[nodiscard]] uint32_t GetCurrentBackbufferId() const
		{
			return m_currentBackbufferId;
		}

		[[nodiscard]] uint32_t GetCurrentFenceId() const
		{
			return m_currentFenceId;
		}

	private:

		void ResetCommandList() const;
		void SetupCamera(const CameraData& camera) const;
		void SetupRenderTarget(const CameraData& camera) const;
		void DrawObjects(std::priority_queue<RenderHandle>& renderQueue, const CameraData& camera) const;
		void FinalizeDrawing() const;
		void ExecuteCommandLists() const;

		ComPtr<IDXGIFactory4> m_dxgiFactory;
		ComPtr<ID3D12Device>  m_device;

		ComPtr<ID3D12Fence>               m_commandFence;
		ComPtr<ID3D12CommandQueue>        m_commandQueue;
		ComPtr<ID3D12CommandAllocator>    m_directCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;

		std::array<ComPtr<ID3D12CommandAllocator>, BUFFER_COUNT> m_frameCommandAllocators;

		ComPtr<IDXGISwapChain>                           m_swapchain;
		std::array<ComPtr<ID3D12Resource>, BUFFER_COUNT> m_swapchainBuffers;

		D3D12_VIEWPORT m_viewport    = {};
		D3D12_RECT     m_scissorRect = {};


		uint32_t m_currentFenceId      = 0;
		uint32_t m_currentBackbufferId = 0;

		uint32_t m_msNumQualityLevels = 0;

		ComPtr<ID3D12Resource> m_depthStencilBuffer;

		uint32_t m_rtvDescriptorSize = 0;
		uint32_t m_dsvDescriptorSize = 0;

		ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
	};
}
