#include "stdafx.h"
#include "StaticSubmesh.h"

#include "Buffer.h"
#include "Context.h"
#include "ResourceUploader.h"
#include "Assets/SubmeshData.h"
#include "Shaders/Shader.h"

namespace Rendering::Dx12
{
	StaticSubmesh::~StaticSubmesh()
	{
		//TODO: release resources
	}

	void StaticSubmesh::Init(const ::Submesh* submesh)
	{
		if (submesh == nullptr)
		{
			return;
		}

		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(submesh->GetVertexBuffer().m_data.size());

		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData = {
			{
				submesh->GetVertexBuffer().m_data.data(),
				(int64_t) submesh->GetVertexBuffer().m_data.size(),
				(int64_t) submesh->GetVertexBuffer().m_data.size()
			}
		};

		ResourceUploader::UploadResource(resourceDesc,
			subresourceData,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			m_vertexUploadBuffer,
			m_vertexBuffer);

		CreateVertexBufferViews(submesh);
		CreateShaderResourceViews(submesh);

		resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(submesh->GetIndexBuffer().m_data.size());

		subresourceData = {
			{
				submesh->GetIndexBuffer().m_data.data(),
				(int64_t) submesh->GetIndexBuffer().m_data.size(),
				(int64_t) submesh->GetIndexBuffer().m_data.size()
			}
		};

		ResourceUploader::UploadResource(resourceDesc, subresourceData, D3D12_RESOURCE_STATE_GENERIC_READ, m_indexUploadBuffer, m_indexBuffer);

		m_indexBufferView = {
			m_indexBuffer->GetGPUVirtualAddress(),
			(uint32_t) submesh->GetIndexBuffer().m_data.size(),
			DXGI_FORMAT_R32_UINT
		};

		m_vertexCount = submesh->GetVertexBuffer().GetElementCount();
		m_indexCount  = submesh->GetIndexBuffer().GetElementCount();
	}

	void StaticSubmesh::Bind(const RenderObject& renderObject) const
	{
		for (auto& [inputSlot, attribute] : renderObject.m_shader->GetInputSlotBindings())
		{
			auto it = m_vertexBufferViews.find(attribute);
			if (it == m_vertexBufferViews.end())
			{
				it = m_vertexBufferViews.find(MeshAttribute::POSITION);
			}
			const D3D12_VERTEX_BUFFER_VIEW* bufferView = &it->second;

			Dx12Context::Get().GetCommandList()->IASetVertexBuffers(inputSlot, 1, bufferView);
		}

		Dx12Context::Get().GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
		Dx12Context::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	const D3D12_VERTEX_BUFFER_VIEW* StaticSubmesh::GetVertexBufferViewForAttribute(const MeshAttribute attribute) const
	{
		const auto it = m_vertexBufferViews.find(attribute);
		if (it == m_vertexBufferViews.end())
		{
			return nullptr;
		}

		return &it->second;
	}

	void StaticSubmesh::CreateVertexBufferViews(const ::Submesh* submesh)
	{
		const AttributeOffsets& offsets = submesh->GetAttributesUsage().GetAttributeOffsets();

		for (uint8_t i = 0; i < (uint8_t) MeshAttribute::UNKNOWN; ++i)
		{
			const MeshAttribute attribute = (MeshAttribute) i;
			AddVertexBufferView(submesh, (MeshAttribute) i, GetAttributeOffset(attribute, offsets), GetAttributeDataSize(attribute, offsets));
		}
	}

	void StaticSubmesh::AddVertexBufferView(const ::Submesh* submesh, const MeshAttribute attribute, const uint16_t offset, const uint16_t dataSize)
	{
		if (dataSize != 0)
		{
			const uint32_t vertexCount = submesh->GetVertexBuffer().GetElementCount();

			m_vertexBufferViews[attribute] =
			{
				m_vertexBuffer->GetGPUVirtualAddress() + vertexCount * offset,
				vertexCount * dataSize,
				dataSize
			};
		}
	}

	void StaticSubmesh::CreateShaderResourceViews(const ::Submesh* submesh)
	{
		const AttributeOffsets& offsets = submesh->GetAttributesUsage().GetAttributeOffsets();

		m_skinningHeapHandle = Dx12Context::Get().CreateHeapHandle(4);

		AddShaderResourceViews(submesh, MeshAttribute::POSITION, offsets.m_positionOffset);
		AddShaderResourceViews(submesh, MeshAttribute::NORMAL, offsets.m_normalOffset);
		AddShaderResourceViews(submesh, MeshAttribute::TANGENT, offsets.m_tangentOffset);
		AddShaderResourceViews(submesh, MeshAttribute::BINORMAL, offsets.m_bitangentOffset);
	}

	void StaticSubmesh::AddShaderResourceViews(const ::Submesh* submesh,
											   const MeshAttribute attribute,
											   const uint16_t offset)
	{
		if (offset != 0 || attribute == MeshAttribute::POSITION)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC desc
			{
				DXGI_FORMAT_R32_FLOAT,
				D3D12_SRV_DIMENSION_BUFFER,
				D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
			};
			desc.Buffer =
			{
				submesh->GetVertexBuffer().GetElementCount() * 3 * offset / 12,
				submesh->GetVertexBuffer().GetElementCount() * 3,
				0,
				D3D12_BUFFER_SRV_FLAG_NONE
			};

			Dx12Context::Get().GetDevice()->CreateShaderResourceView(m_vertexBuffer.Get(), &desc,
				m_skinningHeapHandle->GetCPUHandle(m_skinnedAttributesCount));

			m_skinnedAttributesCount++;
		}
	}

	std::shared_ptr<DescriptorHeapHandle> StaticSubmesh::GetSkinningHeapHandle()
	{
		return m_skinningHeapHandle;
	}

	uint32_t StaticSubmesh::GetSkinnedAttributeCount() const
	{
		return m_skinnedAttributesCount;
	}

	NativeSubmesh* StaticSubmesh::Create(const ::Submesh* submesh)
	{
		StaticSubmesh* const nativeSubmesh = new StaticSubmesh();
		nativeSubmesh->Init(submesh);
		return nativeSubmesh;
	}
}
