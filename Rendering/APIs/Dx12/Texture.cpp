#include "stdafx.h"
#include "Texture.h"
#include "Assets/Texture.h"

#include "Context.h"
#include "Renderer.h"
#include "ResourceUploader.h"

namespace Rendering::Dx12
{
	Texture::~Texture()
	{
		RELEASE_DX(m_texture);
		RELEASE_DX(m_textureUploadBuffer);
	}

	void Texture::Init(const ::Texture* texture)
	{
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels           = 1;
		textureDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width               = texture->GetWidth();
		textureDesc.Height              = texture->GetHeight();
		textureDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize    = 1;
		textureDesc.SampleDesc.Count    = 1;
		textureDesc.SampleDesc.Quality  = 0;
		textureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData                  = texture->GetData();
		textureData.RowPitch               = (uint64_t) texture->GetWidth() * 4;
		textureData.SlicePitch             = textureData.RowPitch * texture->GetHeight();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format                          = textureDesc.Format;
		srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels             = 1;


		ResourceUploader::UploadResource(textureDesc, { textureData }, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			m_textureUploadBuffer, m_texture);

		m_heapHandle = Dx12Context::Get().CreateHeapHandle();

		Dx12Context::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc,
			m_heapHandle->GetCPUHandle());

		SetDebugName(m_texture, texture->GetPath().data());
	}

	void Texture::Bind(const uint32_t index)
	{
		static uint64_t lastAssignedFenceId = 0xFFFFFFFFFFFFFFFF;
		if (lastAssignedFenceId != Renderer::Get().GetCurrentBackbufferId())
		{
			lastAssignedFenceId = Renderer::Get().GetCurrentBackbufferId();
		}

		const uint64_t incrementSize = Dx12Context::Get().GetDevice()->
														  GetDescriptorHandleIncrementSize(
															  D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		Dx12Context::Get().GetCommandList()->SetGraphicsRootDescriptorTable(index, m_heapHandle->GetGPUHandle());
	}

	NativeTexture* Texture::Create(const std::shared_ptr<::Texture>& texture)
	{
		Texture* nativeSubmesh = new Texture();
		nativeSubmesh->Init(texture.get());
		return static_cast<NativeTexture*>(nativeSubmesh);
	}
}

namespace Rendering
{
	extern void Deleter(NativeTexture* texture)
	{
		if (texture != nullptr)
		{
			delete texture;
		}
	}
}
