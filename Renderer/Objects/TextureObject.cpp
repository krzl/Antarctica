#include "stdafx.h"
#include "TextureObject.h"

#include "RenderSystem.h"
#include "ResourceUploader.h"

namespace Renderer
{
	ComPtr<ID3D12DescriptorHeap> TextureObject::m_heap;
	uint32_t TextureObject::m_counter = 0;

	void TextureObject::InitHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors             = 128; //TODO: allocate additional heaps when limit is reached
		srvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		RenderSystem::Get().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_heap));
	}

	void TextureObject::Init()
	{
		if (m_heap.Get() == nullptr)
		{
			InitHeap();
		}

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels           = 1;
		textureDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width               = m_width;
		textureDesc.Height              = m_height;
		textureDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize    = 1;
		textureDesc.SampleDesc.Count    = 1;
		textureDesc.SampleDesc.Quality  = 0;
		textureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData                  = m_data;
		textureData.RowPitch               = (uint64_t) m_width * m_channels;
		textureData.SlicePitch             = textureData.RowPitch * m_height;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format                          = textureDesc.Format;
		srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels             = 1;

		m_id = m_counter++;

		const uint64_t incrementSize = RenderSystem::Get().GetDevice()->
														   GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += incrementSize * m_id;

		ResourceUploader::UploadResource(textureDesc, { textureData }, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
										 m_textureUploadBuffer, m_texture);

		RenderSystem::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, handle);
	}

	void TextureObject::Bind(const uint32_t index)
	{
		if (!m_isInitialized)
		{
			Init();
			m_isInitialized = true;
		}

		static uint64_t lastAssignedFenceId = 0xFFFFFFFFFFFFFFFF;
		if (lastAssignedFenceId != RenderSystem::Get().GetCurrentBackbufferId())
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_heap.Get() };
			RenderSystem::Get().GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			lastAssignedFenceId = RenderSystem::Get().GetCurrentBackbufferId();
		}

		const uint64_t incrementSize = RenderSystem::Get().GetDevice()->
														   GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_GPU_DESCRIPTOR_HANDLE handle(m_heap->GetGPUDescriptorHandleForHeapStart());
		handle.ptr += incrementSize * m_id;

		RenderSystem::Get().GetCommandList()->SetGraphicsRootDescriptorTable(index, handle);
	}
}
