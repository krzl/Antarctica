#pragma once

#include "Common.h"
#include "DescriptorHeap.h"
#include "RenderSystem.h"
#include "ScratchBuffer.h"
#include "APIs/IContext.h"

class Material;

namespace Renderer::Dx12
{
	struct RenderObject
	{
		IShader*                      m_shader;
		ISubmesh*                     m_submesh;
		ScratchBufferHandle           m_perObjectBuffer;
		ScratchBufferHandle           m_perCallBuffer;
		std::map<uint32_t, ITexture*> m_textures;
		uint32_t                      m_instanceCount;
		
		std::shared_ptr<DescriptorHeapHandle> m_skinningBufferHandle;
		std::shared_ptr<DescriptorHeapHandle> m_boneTransforms;
		std::shared_ptr<DescriptorHeapHandle> m_weightsBuffer;
	};

	class Dx12Context final : public IContext
	{
	public:

		static Dx12Context& Get();
		explicit            Dx12Context();

		void Init(const Platform::Window& window, const Settings& settings) override;
		void FlushCommandQueue() override;
		void OnResize(const Platform::Window& window) override;

		void WaitForFrameCompletion() override;

		void CreateRenderQueue(RenderQueue& objectsToRender) override;

		void        SetupCamera(const Renderer::CameraData& camera) const override;
		void        SetupRenderTarget(const CameraData& camera) const override;
		void        DrawObjects(const CameraData& camera) override;
		void        FinalizeDrawing() override;
		void        UpdateSkinning() override;
		static void UpdateMaterial(const Material* material, RenderObject& renderObject);

		uint32_t GetCurrentBackbufferId() override;

		void ExecuteAndPresent() override;
		void Cleanup() override;

		std::shared_ptr<DescriptorHeapHandle> CreateHeapHandle(uint32_t                    size  = 1,
															   D3D12_DESCRIPTOR_HEAP_FLAGS flags =
																   D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

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

		[[nodiscard]] ScratchBuffer& GetScratchBuffer()
		{
			return m_scratchBuffer;
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

		std::vector<std::shared_ptr<DescriptorHeap>> m_srvDescriptorHeaps;
		ComPtr<ID3D12DescriptorHeap>                 m_rtvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap>                 m_dsvDescriptorHeap;


		std::vector<RenderObject> m_renderQueue;

		ScratchBuffer m_scratchBuffer;

		uint32_t m_msNumQualityLevels = 0;

		uint32_t m_currentFenceId      = 0;
		uint32_t m_currentBackbufferId = 0;
	};
}
