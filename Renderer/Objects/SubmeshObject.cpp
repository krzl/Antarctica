#include "stdafx.h"
#include "SubmeshObject.h"

#include "RenderSystem.h"
#include "ResourceUploader.h"
#include "SubmeshData.h"

namespace Renderer
{
	void SubmeshObject::Init()
	{
		if (m_submesh == nullptr)
		{
			return;
		}

		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_submesh->GetVertexBuffer().m_data.size());
		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData = {
			{
				m_submesh->GetVertexBuffer().m_data.data(),
				(int64_t) m_submesh->GetVertexBuffer().m_data.size(),
				(int64_t) m_submesh->GetVertexBuffer().m_data.size()
			}
		};

		ResourceUploader::UploadResource(resourceDesc, subresourceData, D3D12_RESOURCE_STATE_GENERIC_READ,
										 m_vertexBuffer, m_vertexUploadBuffer);

		m_vertexBufferView = {
			m_vertexBuffer->GetGPUVirtualAddress(),
			(uint32_t) m_submesh->GetVertexBuffer().m_data.size(),
			(uint32_t) m_submesh->GetVertexBuffer().m_elementSize
		};


		resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_submesh->GetIndexBuffer().m_data.size());

		subresourceData = {
			{
				m_submesh->GetIndexBuffer().m_data.data(),
				(int64_t) m_submesh->GetIndexBuffer().m_data.size(),
				(int64_t) m_submesh->GetIndexBuffer().m_data.size()
			}
		};

		ResourceUploader::UploadResource(resourceDesc, subresourceData, D3D12_RESOURCE_STATE_GENERIC_READ,
										 m_indexBuffer, m_indexUploadBuffer);

		m_indexBufferView = {
			m_indexBuffer->GetGPUVirtualAddress(),
			(uint32_t) m_submesh->GetIndexBuffer().m_data.size(),
			DXGI_FORMAT_R32_UINT
		};

		m_indexCount = (uint32_t) m_submesh->GetIndexBuffer().m_data.size() / m_submesh->GetIndexBuffer().m_elementSize;
	}

	void SubmeshObject::Bind()
	{
		if (!m_isInitialized)
		{
			Init();
			m_isInitialized = true;
		}

		RenderSystem::Get().GetCommandList()->IASetVertexBuffers(0u, 1, &m_vertexBufferView);
		RenderSystem::Get().GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
		RenderSystem::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}
