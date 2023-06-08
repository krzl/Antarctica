#include "stdafx.h"
#include "Context.h"

#include <Window.h>

#include "AssetManager.h"
#include "Buffer.h"
#include "CameraData.h"
#include "RenderObject.h"
#include "Submesh.h"
#include "Texture.h"
#include "Assets/SubmeshData.h"
#include "Buffers/ConstantBuffer.h"
#include "Shaders/ComputeShader.h"
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

	void Dx12Context::WaitForFrameCompletion()
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

		m_scratchBuffer.Reset();
		DescriptorHeap::OnFrameStart();
	}

	void Dx12Context::CreateRenderQueue(std::multiset<QueuedRenderObject>& objectsToRender)
	{
		m_renderQueue.clear();
		m_renderQueue.reserve(objectsToRender.size());

		bool foundAnyMeshToSkin = false;

		std::map<const Skeleton*, std::shared_ptr<DescriptorHeapHandle>> weightBufferHandles;

		std::vector<uint8_t> accumulatedConstantBuffers;

		const QueuedRenderObject* previousQueuedObject = nullptr;
		uint32_t                  instanceCount        = 1;

		for (const QueuedRenderObject& queuedObject : objectsToRender)
		{
			if (previousQueuedObject)
			{
				if (previousQueuedObject->m_submesh == queuedObject.m_submesh &&
					//previousQueuedObject->m_material == queuedObject.m_material &&
					previousQueuedObject->m_boneTransforms.size() == 0)
				{
					accumulatedConstantBuffers.insert(accumulatedConstantBuffers.end(),
													  queuedObject.m_constantBuffer.begin(),
													  queuedObject.m_constantBuffer.end());

					++instanceCount;
					continue;
				}
				else
				{
					RenderObject* lastRenderObject     = &m_renderQueue[m_renderQueue.size() - 1];
					lastRenderObject->m_constantBuffer = GetScratchBuffer().CreateHandle(
						(uint32_t) accumulatedConstantBuffers.size(),
						accumulatedConstantBuffers.data());
					accumulatedConstantBuffers.clear();

					lastRenderObject->m_instanceCount = instanceCount;
					instanceCount                     = 1;
				}
			}

			previousQueuedObject = &queuedObject;

			RenderObject& renderObject = m_renderQueue.emplace_back();

			std::shared_ptr<::Shader> shader = queuedObject.m_material->GetShader();
			if (shader->GetNativeObject() == nullptr)
			{
				shader->SetNativeObject(Shader::Create(shader));
			}
			renderObject.m_shader = shader->GetNativeObject();

			UpdateMaterial(queuedObject.m_material, renderObject);

			accumulatedConstantBuffers.insert(accumulatedConstantBuffers.end(),
											  queuedObject.m_constantBuffer.begin(),
											  queuedObject.m_constantBuffer.end());

			if (queuedObject.m_submesh->GetNativeObject() == nullptr)
			{
				queuedObject.m_submesh->SetNativeObject(Submesh::Create(queuedObject.m_submesh));
			}
			renderObject.m_submesh = queuedObject.m_submesh->GetNativeObject();

			if (queuedObject.m_boneTransforms.size() != 0)
			{
				if (!foundAnyMeshToSkin)
				{
					foundAnyMeshToSkin = true;
					if (m_skinningShader == nullptr)
					{
						m_skinningShader = AssetManager::GetAsset<::ComputeShader>(
							"../Resources/Shaders/Compute/skinning.hlsl");
						m_skinningShader->SetNativeObject(IComputeShader::Create(m_skinningShader));
					}
				}

				const Skeleton&   skeleton     = queuedObject.m_submesh->GetSkeleton();
				const MeshBuffer& vertexBuffer = queuedObject.m_submesh->GetVertexBuffer();

				auto it = weightBufferHandles.find(&skeleton);
				if (it != weightBufferHandles.end())
				{
					renderObject.m_weightsBuffer = it->second;
				}
				else
				{
					const std::shared_ptr<DescriptorHeapHandle> weightBufferHandle = GetScratchBuffer().CreateSRV(
						sizeof(VertexWeights),
						(uint32_t) skeleton.m_vertexWeights.size(), skeleton.m_vertexWeights.data());
					renderObject.m_weightsBuffer   = weightBufferHandle;
					weightBufferHandles[&skeleton] = weightBufferHandle;
				}

				if (!queuedObject.m_skinningBuffer->IsInitialized())
				{
					queuedObject.m_skinningBuffer->InitUAV(
						vertexBuffer.m_elementCount * renderObject.m_submesh->GetSkinnedAttributeCount(),
						sizeof(float) * 3);
				}
				else
				{
					const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
						queuedObject.m_skinningBuffer->GetCurrentBuffer()->GetBuffer().Get(),
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				}

				renderObject.m_skinningBuffer = queuedObject.m_skinningBuffer->GetCurrentBuffer();

				renderObject.m_boneTransforms =
					GetScratchBuffer().CreateSRV(sizeof(float) * 16,
												 (uint32_t) skeleton.m_bones.size(),
												 queuedObject.m_boneTransforms.data());
			}
		}

		if (!m_renderQueue.empty())
		{
			RenderObject* lastRenderObject = &m_renderQueue[m_renderQueue.size() - 1];
			lastRenderObject->m_constantBuffer = GetScratchBuffer().CreateHandle(
				(uint32_t)accumulatedConstantBuffers.size(),
				accumulatedConstantBuffers.data());
			accumulatedConstantBuffers.clear();

			lastRenderObject->m_instanceCount = instanceCount;
		}
		
		m_scratchBuffer.SubmitBuffers();
	}

	void Dx12Context::UpdateMaterial(const Material* material, RenderObject& renderObject)
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
				renderObject.m_textures[id] = texture->GetNativeObject();
			}
		}
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

	void Dx12Context::DrawObjects(const CameraData& camera)
	{
		for (RenderObject& renderObject : m_renderQueue)
		{
			renderObject.m_shader->Bind();

			for (const auto& texture : renderObject.m_textures)
			{
				texture.second->Bind(texture.first);
			}

			m_commandList->SetGraphicsRootConstantBufferView(
				0, GetScratchBuffer().GetGpuPointer(renderObject.m_constantBuffer));
			camera.m_constantBuffer->UpdateAndGetCurrentBuffer()->Bind(1); //TODO: bind only when it wasn't bound before

			renderObject.m_submesh->Bind(renderObject.m_shader, renderObject.m_skinningBuffer);

			m_commandList->DrawIndexedInstanced(renderObject.m_submesh->GetIndexCount(), renderObject.m_instanceCount,
												0, 0, 0);
		}
	}

	void Dx12Context::FinalizeDrawing()
	{
		const D3D12_RESOURCE_BARRIER transitionToPresent =
			CD3DX12_RESOURCE_BARRIER::Transition(m_swapchainBuffers[m_currentBackbufferId].Get(),
												 D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &transitionToPresent);
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

	std::shared_ptr<DescriptorHeapHandle> Dx12Context::CreateHeapHandle(
		const uint32_t size, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		for (const std::shared_ptr<DescriptorHeap>& heap : m_srvDescriptorHeaps)
		{
			if (heap->m_flags == flags)
			{
				std::shared_ptr<DescriptorHeapHandle> handle = heap->GetNextHandle(size);
				if (handle != nullptr)
				{
					return handle;
				}
			}
		}

		const std::shared_ptr<DescriptorHeap>& heap = m_srvDescriptorHeaps.emplace_back(new DescriptorHeap());
		heap->Init(flags);

		return heap->GetNextHandle(size);
	}

	void Dx12Context::Cleanup()
	{
		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT - 1; ++i)
		{
			FlushCommandQueue();
		}
	}
}
