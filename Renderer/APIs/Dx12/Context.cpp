#include "stdafx.h"
#include "Context.h"

#include <Window.h>

#include "Buffer.h"
#include "CameraData.h"
#include "RenderHandle.h"
#include "Submesh.h"
#include "Texture.h"
#include "Assets/SubmeshData.h"
#include "Buffers/ConstantBuffer.h"
#include "Shaders/Shader.h"

namespace Renderer::Dx12
{
	static Dx12Context* context;

	Dx12Context& Dx12Context::Get()
	{
		return *context;
	}

	Dx12Context::Dx12Context()
	{
		context = this;
	}

	void Dx12Context::Init(const Platform::Window& window, const Settings& settings)
	{
#if defined(DEBUG) | defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}

			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
			{
				CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));

				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
			}
		}
#else
		CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
#endif

		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_commandFence));

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {
			DXGI_FORMAT_R8G8B8A8_UNORM,
			4,
			D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
			0
		};

		m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,
									  sizeof(msQualityLevels));

		m_msNumQualityLevels = msQualityLevels.NumQualityLevels;

		constexpr D3D12_COMMAND_QUEUE_DESC commandQueueInfo = {
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			0,
			D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};

		m_device->CreateCommandQueue(&commandQueueInfo, IID_PPV_ARGS(&m_commandQueue));
		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_directCommandAllocator));
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_directCommandAllocator.Get(), nullptr,
									IID_PPV_ARGS(&m_commandList));

		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT; ++i)
		{
			m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
											 IID_PPV_ARGS(&m_frameCommandAllocators[i]));
		}

		DXGI_SWAP_CHAIN_DESC swapChainInfo = {
			{
				window.GetWidth(),
				window.GetHeight(),
				{
					60,
					1
				},
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				DXGI_MODE_SCALING_UNSPECIFIED
			},
			{
				1,
				m_msNumQualityLevels - 1
			},
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			RenderSystem::BUFFER_COUNT,
			window.GetHandle(),
			true,
			DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		m_dxgiFactory->CreateSwapChain(m_commandQueue.Get(), &swapChainInfo, m_swapchain.GetAddressOf());

		constexpr D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapInfo = {
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			RenderSystem::BUFFER_COUNT,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		m_device->CreateDescriptorHeap(&rtvDescriptorHeapInfo, IID_PPV_ARGS(&m_rtvDescriptorHeap));

		constexpr D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapInfo = {
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		m_device->CreateDescriptorHeap(&dsvDescriptorHeapInfo, IID_PPV_ARGS(&m_dsvDescriptorHeap));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		m_commandList->Close();
		ID3D12CommandList* commandLists = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(1, &commandLists);

		FlushCommandQueue();
	}

	void Dx12Context::FlushCommandQueue()
	{
		++m_currentFenceId;

		m_commandQueue->Signal(m_commandFence.Get(), m_currentFenceId);

		if (m_commandFence->GetCompletedValue() <= m_currentFenceId)
		{
			const HANDLE event = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_commandFence->SetEventOnCompletion(m_currentFenceId, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}

	void Dx12Context::OnResize(const Platform::Window& window)
	{
		FlushCommandQueue();

		m_commandList->Reset(m_directCommandAllocator.Get(), nullptr);

		for (auto& buffer : m_swapchainBuffers)
		{
			buffer.Reset();
		}
		m_depthStencilBuffer.Reset();

		m_currentBackbufferId = 0;

		m_swapchain->ResizeBuffers(RenderSystem::BUFFER_COUNT, window.GetWidth(), window.GetHeight(),
								   DXGI_FORMAT_R8G8B8A8_UNORM,
								   DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int32_t i = 0; i < RenderSystem::BUFFER_COUNT; ++i)
		{
			m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_swapchainBuffers[i]));
			m_device->CreateRenderTargetView(m_swapchainBuffers[i].Get(), nullptr, rtvDescriptorHandle);
			rtvDescriptorHandle.Offset(1, m_rtvDescriptorSize);
		}

		const D3D12_RESOURCE_DESC depthStencilInfo = {
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			window.GetWidth(),
			window.GetHeight(),
			1,
			1,
			DXGI_FORMAT_R24G8_TYPELESS,
			{
				1,
				m_msNumQualityLevels - 1
			},
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		};

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			DXGI_FORMAT_D24_UNORM_S8_UINT,
		};

		depthStencilClearValue.DepthStencil = {
			1.0f,
			0
		};

		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		m_device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilInfo,
			D3D12_RESOURCE_STATE_COMMON,
			&depthStencilClearValue,
			IID_PPV_ARGS(&m_depthStencilBuffer)
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvInfo = {
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			D3D12_DSV_DIMENSION_TEXTURE2D,
			D3D12_DSV_FLAG_NONE,
		};

		dsvInfo.Texture2D = {
			0
		};

		m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvInfo,
										 m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		const D3D12_RESOURCE_BARRIER depthTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),
																							D3D12_RESOURCE_STATE_COMMON,
																							D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_commandList->ResourceBarrier(1, &depthTransition);

		m_viewport = {
			0.0f,
			0.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.0f,
			1.0f
		};

		m_scissorRect = {
			0,
			0,
			static_cast<long>(window.GetWidth()),
			static_cast<long>(window.GetHeight())
		};

		m_commandList->Close();
		ID3D12CommandList* commandLists = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(1, &commandLists);
		FlushCommandQueue();
	}

	void Dx12Context::WaitForFrameCompletion() const
	{
		if (m_commandFence->GetCompletedValue() <= m_currentFenceId - RenderSystem::BUFFER_COUNT)
		{
			const HANDLE event = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_commandFence->SetEventOnCompletion(m_currentFenceId, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		m_frameCommandAllocators[m_currentBackbufferId]->Reset();
		m_commandList->Reset(m_frameCommandAllocators[m_currentBackbufferId].Get(), nullptr);

		DescriptorHeap::OnFrameStart();
	}

	void Dx12Context::SetupCamera(const CameraData& camera) const
	{
		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);
	}

	void Dx12Context::SetupRenderTarget(const CameraData& camera) const
	{
		const D3D12_RESOURCE_BARRIER transitionToRender =
			CD3DX12_RESOURCE_BARRIER::Transition(m_swapchainBuffers[m_currentBackbufferId].Get(),
												 D3D12_RESOURCE_STATE_PRESENT,
												 D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &transitionToRender);

		const auto backbufferView = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackbufferId,
			m_rtvDescriptorSize);

		constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_commandList->ClearRenderTargetView(backbufferView, clearColor, 0, nullptr);

		const auto depthStencilView =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
											 0, 0,
											 nullptr);

		m_commandList->OMSetRenderTargets(1, &backbufferView, true, &depthStencilView);
	}

	void Dx12Context::DrawObjects(std::priority_queue<RenderHandle>& renderQueue, const CameraData& camera)
	{
		while (!renderQueue.empty())
		{
			const RenderHandle& renderable = renderQueue.top();

			std::shared_ptr<::Shader> shader = renderable.m_material->GetShader();
			if (shader->GetNativeObject() == nullptr)
			{
				shader->SetNativeObject(Shader::Create(shader));
			}
			shader->GetNativeObject()->Bind();

			UpdateAndBindMaterial(renderable.m_material);

			renderable.m_constantBuffer->UpdateAndGetCurrentBuffer()->Bind(0);
			camera.m_constantBuffer->UpdateAndGetCurrentBuffer()->Bind(1); //TODO: bind only when it wasn't bound before

			const ::Submesh* submesh = renderable.m_submesh;
			if (submesh->GetNativeObject() == nullptr)
			{
				submesh->SetNativeObject(Submesh::Create(submesh));
			}
			
			renderable.m_submesh->GetNativeObject()->Bind(shader->GetNativeObject(), renderable.m_skinningBuffers);

			m_commandList->DrawIndexedInstanced(renderable.m_submesh->GetNativeObject()->GetIndexCount(), 1, 0, 0, 0);

			renderQueue.pop();
		}
	}

	void Dx12Context::FinalizeDrawing()
	{
		const D3D12_RESOURCE_BARRIER transitionToPresent =
			CD3DX12_RESOURCE_BARRIER::Transition(m_swapchainBuffers[m_currentBackbufferId].Get(),
												 D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &transitionToPresent);
	}

	void Dx12Context::UpdateAndBindMaterial(const Material* material)
	{
		const ShaderDescriptor& descriptor = material->GetShader()->GetNativeObject()->GetShaderDescriptor();

		for (const auto& [name, id] : descriptor.GetTextures())
		{
			auto it = material->GetTextures().find(name);
			if (it != material->GetTextures().end())
			{
				std::shared_ptr<::Texture> texture = it->second;
				if (texture->GetNativeObject() == nullptr)
				{
					texture->SetNativeObject(Texture::Create(texture));
				}
				texture->GetNativeObject()->Bind(id);
			}
		}
	}

	uint32_t Dx12Context::GetCurrentBackbufferId()
	{
		return m_currentBackbufferId;
	}

	void Dx12Context::ExecuteAndPresent()
	{
		m_commandList->Close();
		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		++m_currentFenceId;
		m_commandQueue->Signal(m_commandFence.Get(), m_currentFenceId);

		m_swapchain->Present(0, 0);
		m_currentBackbufferId = (m_currentBackbufferId + 1) % RenderSystem::BUFFER_COUNT;
	}

	std::shared_ptr<DescriptorHeapHandle> Dx12Context::CreateHeapHandle()
	{
		for (std::shared_ptr<DescriptorHeap>& heap : m_srvDescriptorHeaps)
		{
			if (!heap->IsFilled())
			{
				return heap->GetNextHandle();
			}
		}

		std::shared_ptr<DescriptorHeap>& heap = m_srvDescriptorHeaps.emplace_back(new DescriptorHeap());
		heap->Init();

		return heap->GetNextHandle();
	}

	void Dx12Context::Cleanup()
	{
		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT - 1; ++i)
		{
			FlushCommandQueue();
		}
	}
}