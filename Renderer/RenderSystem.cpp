#include "stdafx.h"
#include "RenderSystem.h"

#include <Window.h>

#include "../AssetManagement/Assets/Shader.h"
#include "Objects/ConstantBuffer.h"

namespace Renderer
{
	static RenderSystem* renderSystem;

	RenderSystem& RenderSystem::Get()
	{
		return *renderSystem;
	}

	void RenderSystem::Init(const Platform::Window& window, const Settings& settings)
	{
		renderSystem = this;

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

		D3D12_COMMAND_QUEUE_DESC commandQueueInfo = {
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			0,
			D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};

		m_device->CreateCommandQueue(&commandQueueInfo, IID_PPV_ARGS(&m_commandQueue));
		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_directCommandAllocator));
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_directCommandAllocator.Get(), nullptr,
									IID_PPV_ARGS(&m_commandList));

		for (uint32_t i = 0; i < BUFFER_COUNT; ++i)
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
			BUFFER_COUNT,
			window.GetHandle(),
			true,
			DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		m_dxgiFactory->CreateSwapChain(m_commandQueue.Get(), &swapChainInfo, m_swapchain.GetAddressOf());

		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapInfo = {
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			BUFFER_COUNT,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		m_device->CreateDescriptorHeap(&rtvDescriptorHeapInfo, IID_PPV_ARGS(&m_rtvDescriptorHeap));

		D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapInfo = {
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

		OnResize(window);
	}

	void RenderSystem::FlushCommandQueue()
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

	void RenderSystem::OnResize(const Platform::Window& window)
	{
		FlushCommandQueue();

		m_commandList->Reset(m_directCommandAllocator.Get(), nullptr);

		for (auto& buffer : m_swapchainBuffers)
		{
			buffer.Reset();
		}
		m_depthStencilBuffer.Reset();

		m_currentBackbufferId = 0;

		m_swapchain->ResizeBuffers(BUFFER_COUNT, window.GetWidth(), window.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM,
								   DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int32_t i = 0; i < BUFFER_COUNT; ++i)
		{
			m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_swapchainBuffers[i]));
			m_device->CreateRenderTargetView(m_swapchainBuffers[i].Get(), nullptr, rtvDescriptorHandle);
			rtvDescriptorHandle.Offset(1, m_rtvDescriptorSize);
		}

		D3D12_RESOURCE_DESC depthStencilInfo = {
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

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
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

		D3D12_RESOURCE_BARRIER depthTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),
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

	void RenderSystem::Render(std::priority_queue<RenderHandle>& renderQueue, std::priority_queue<CameraData>& cameras)
	{
		if (m_commandFence->GetCompletedValue() <= m_currentFenceId - BUFFER_COUNT)
		{
			const HANDLE event = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_commandFence->SetEventOnCompletion(m_currentFenceId, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		ResetCommandList();

		while (!cameras.empty())
		{
			const CameraData& camera = cameras.top();
			
			SetupCamera(camera);
			SetupRenderTarget(camera);
			DrawObjects(renderQueue, camera);
			FinalizeDrawing();

			cameras.pop();
		}

		ExecuteCommandLists();

		Present();
	}

	void RenderSystem::ResetCommandList()
	{
		m_frameCommandAllocators[m_currentBackbufferId]->Reset();
		m_commandList->Reset(m_frameCommandAllocators[m_currentBackbufferId].Get(), nullptr);

		//ID3D12DescriptorHeap* descriptorHeaps[] = { GetHeap().Get() };
		//m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	}

	void RenderSystem::SetupCamera(const CameraData& camera)
	{
		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);
	}

	void RenderSystem::SetupRenderTarget(const CameraData& camera)
	{
		D3D12_RESOURCE_BARRIER transitionToRender =
			CD3DX12_RESOURCE_BARRIER::Transition(m_swapchainBuffers[m_currentBackbufferId].Get(),
												 D3D12_RESOURCE_STATE_PRESENT,
												 D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &transitionToRender);

		auto backbufferView = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
															m_currentBackbufferId,
															m_rtvDescriptorSize);

		const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_commandList->ClearRenderTargetView(backbufferView, clearColor, 0, nullptr);

		auto depthStencilView =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
											 0, 0,
											 nullptr);

		m_commandList->OMSetRenderTargets(1, &backbufferView, true, &depthStencilView);
	}

	void RenderSystem::DrawObjects(std::priority_queue<RenderHandle>& renderQueue, const CameraData& camera)
	{
		while (!renderQueue.empty())
		{
			const RenderHandle& renderable = renderQueue.top();
			
			renderable.m_material->GetShaderObject().Bind(*renderable.m_attributeUsage);
			renderable.m_material->UpdateAndBind();

			renderable.m_constantBuffer->Bind(m_commandList.Get(), 0);
			camera.m_constantBuffer->Bind(m_commandList.Get(), 1); //TODO: bind only when it wasn't bound before

			renderable.m_submesh->Bind();

			m_commandList->DrawIndexedInstanced(renderable.m_submesh->m_indexCount, 1, 0, 0, 0);

			renderQueue.pop();
		}
	}

	void RenderSystem::FinalizeDrawing()
	{
		D3D12_RESOURCE_BARRIER transitionToPresent =
			CD3DX12_RESOURCE_BARRIER::Transition(m_swapchainBuffers[m_currentBackbufferId].Get(),
												 D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &transitionToPresent);
	}

	void RenderSystem::ExecuteCommandLists()
	{
		m_commandList->Close();
		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	void RenderSystem::Present()
	{
		++m_currentFenceId;
		m_commandQueue->Signal(m_commandFence.Get(), m_currentFenceId);

		m_swapchain->Present(0, 0);
		m_currentBackbufferId = (m_currentBackbufferId + 1) % BUFFER_COUNT;
	}

	void RenderSystem::Cleanup()
	{
		for (uint32_t i = 0; i < BUFFER_COUNT - 1; ++i)
		{
			FlushCommandQueue();
		}
	}
}
