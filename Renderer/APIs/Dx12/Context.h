#pragma once

#include "Common.h"
#include "DescriptorHeap.h"
#include "RenderSystem.h"
#include "APIs/IContext.h"

class Material;

namespace Renderer::Dx12
{
	class Dx12Context final : public IContext
	{
	public:

		static Dx12Context& Get();

		explicit Dx12Context();

		void Init(const Platform::Window& window, const Settings& settings) override;
		void FlushCommandQueue() override;
		void OnResize(const Platform::Window& window) override;

		void        WaitForFrameCompletion() const override;
		void        SetupCamera(const Renderer::CameraData& camera) const override;
		void        SetupRenderTarget(const CameraData& camera) const override;
		void        DrawObjects(std::priority_queue<RenderHandle>& renderQueue, const CameraData& camera) override;
		void        FinalizeDrawing() override;
		void        UpdateSkinning(IComputeShader* computeShader, std::vector<SkinningData>& skinningData) override;
		static void UpdateAndBindMaterial(const Material* material);

		uint32_t GetCurrentBackbufferId() override;

		void ExecuteAndPresent() override;
		void Cleanup() override;

		std::shared_ptr<DescriptorHeapHandle> CreateHeapHandle();

		ID3D12Device* GetDevice() const
		{
			return m_device.Get();
		}

		[[nodiscard]] IDXGIFactory4* GetDXGIFactory() const
		{
			return m_dxgiFactory.Get();
		}

		[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const
		{
			return m_commandList.Get();
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

		ComPtr<IDXGIFactory4> m_dxgiFactory;
		ComPtr<ID3D12Device>  m_device;

		ComPtr<ID3D12Fence>               m_commandFence;
		ComPtr<ID3D12CommandQueue>        m_commandQueue;
		ComPtr<ID3D12CommandAllocator>    m_directCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;

		std::array<ComPtr<ID3D12CommandAllocator>, RenderSystem::BUFFER_COUNT> m_frameCommandAllocators;

		ComPtr<IDXGISwapChain>                                         m_swapchain;
		std::array<ComPtr<ID3D12Resource>, RenderSystem::BUFFER_COUNT> m_swapchainBuffers;

		D3D12_VIEWPORT m_viewport    = {};
		D3D12_RECT     m_scissorRect = {};

		ComPtr<ID3D12Resource> m_depthStencilBuffer;

		uint32_t m_rtvDescriptorSize = 0;
		uint32_t m_dsvDescriptorSize = 0;

		std::vector<std::shared_ptr<DescriptorHeap>>  m_srvDescriptorHeaps;
		ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

		uint32_t m_msNumQualityLevels = 0;

		uint32_t m_currentFenceId      = 0;
		uint32_t m_currentBackbufferId = 0;
	};
}
