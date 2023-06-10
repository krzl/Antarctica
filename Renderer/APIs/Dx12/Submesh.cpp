#include "stdafx.h"
#include "Submesh.h"

#include "Buffer.h"
#include "Context.h"
#include "ResourceUploader.h"
#include "Assets/SubmeshData.h"
#include "Shaders/Shader.h"

namespace Renderer::Dx12
{
	MeshAttribute GetMeshAttributeFromName(const char* attributeName, const uint32_t index)
	{
		struct StrCmp
		{
			bool operator()(char const* a, char const* b) const
			{
				return std::strcmp(a, b) < 0;
			}
		};

		if (std::strcmp(attributeName, "TEXCOORD") == 0)
		{
			static std::vector texcoords = {
				MeshAttribute::TEXCOORD0,
				MeshAttribute::TEXCOORD1,
				MeshAttribute::TEXCOORD2,
				MeshAttribute::TEXCOORD3
			};

			return texcoords[index];
		}

		if (std::strcmp(attributeName, "COLOR") == 0)
		{
			static std::vector colors = {
				MeshAttribute::COLOR0,
				MeshAttribute::COLOR1,
				MeshAttribute::COLOR2,
				MeshAttribute::COLOR3
			};

			return colors[index];
		}

		static std::map<const char*, MeshAttribute, StrCmp> attributeMap =
		{
			{ "POSITION", MeshAttribute::POSITION },
			{ "NORMAL", MeshAttribute::NORMAL },
			{ "TANGENT", MeshAttribute::TANGENT },
			{ "BINORMAL", MeshAttribute::BINORMAL }
		};

		const auto it = attributeMap.find(attributeName);
		if (it != attributeMap.end())
		{
			return it->second;
		}

		return MeshAttribute::UNKNOWN;
	}

	Submesh::~Submesh()
	{
		RELEASE_DX(m_indexBuffer);
		RELEASE_DX(m_indexUploadBuffer);
		RELEASE_DX(m_vertexBuffer);
		RELEASE_DX(m_vertexUploadBuffer);
	}

	void Submesh::Init(const ::Submesh* submesh)
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

		ResourceUploader::UploadResource(resourceDesc, subresourceData, D3D12_RESOURCE_STATE_GENERIC_READ,
										 m_vertexUploadBuffer, m_vertexBuffer);

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

		ResourceUploader::UploadResource(resourceDesc, subresourceData, D3D12_RESOURCE_STATE_GENERIC_READ,
										 m_indexUploadBuffer, m_indexBuffer);

		m_indexBufferView = {
			m_indexBuffer->GetGPUVirtualAddress(),
			(uint32_t) submesh->GetIndexBuffer().m_data.size(),
			DXGI_FORMAT_R32_UINT
		};

		m_vertexCount = submesh->GetVertexBuffer().m_elementCount;
		m_indexCount  = submesh->GetIndexBuffer().m_elementCount;
	}

	auto Submesh::Bind(const RenderObject& renderObject) const -> void
	{
		uint32_t i = 0;

		for (auto& [inputSlot, attribute] : renderObject.m_shader->GetInputSlotBindings())
		{
			auto it = m_vertexBufferViews.find(attribute);
			if (it == m_vertexBufferViews.end())
			{
				it = m_vertexBufferViews.find(MeshAttribute::POSITION);
			}
			const D3D12_VERTEX_BUFFER_VIEW* bufferView = &it->second;

			Dx12Context::Get().GetCommandList()->IASetVertexBuffers(inputSlot, 1, bufferView);
			++i;
		}

		Dx12Context::Get().GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
		Dx12Context::Get().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	const D3D12_VERTEX_BUFFER_VIEW* Submesh::GetVertexBufferViewForAttribute(MeshAttribute attribute) const
	{
		const auto it = m_vertexBufferViews.find(attribute);
		if (it == m_vertexBufferViews.end())
		{
			return nullptr;
		}

		return &it->second;
	}

	void Submesh::CreateVertexBufferViews(const ::Submesh* submesh)
	{
		const AttributeOffsets& offsets = submesh->GetAttributes().GetAttributeOffsets();

		AddVertexBufferView(submesh, MeshAttribute::POSITION, offsets.m_positionOffset, 0);
		AddVertexBufferView(submesh, MeshAttribute::NORMAL, offsets.m_normalOffset, 0);
		AddVertexBufferView(submesh, MeshAttribute::TANGENT, offsets.m_tangentOffset, 0);
		AddVertexBufferView(submesh, MeshAttribute::BINORMAL, offsets.m_bitangentOffset, 0);
		AddVertexBufferView(submesh, MeshAttribute::COLOR0, offsets.m_colorOffset0, 0);
		AddVertexBufferView(submesh, MeshAttribute::COLOR1, offsets.m_colorOffset1, 0);
		AddVertexBufferView(submesh, MeshAttribute::COLOR2, offsets.m_colorOffset2, 0);
		AddVertexBufferView(submesh, MeshAttribute::COLOR3, offsets.m_colorOffset3, 0);
		AddVertexBufferView(submesh, MeshAttribute::TEXCOORD0, offsets.m_texcoordOffset0, offsets.m_texcoordOffset1);
		AddVertexBufferView(submesh, MeshAttribute::TEXCOORD1, offsets.m_texcoordOffset1, offsets.m_texcoordOffset2);
		AddVertexBufferView(submesh, MeshAttribute::TEXCOORD2, offsets.m_texcoordOffset2, offsets.m_texcoordOffset3);
		AddVertexBufferView(submesh, MeshAttribute::TEXCOORD3, offsets.m_texcoordOffset3, offsets.m_stride);
	}

	static uint32_t GetAttributeDataSize(const MeshAttribute attribute, const uint16_t  offset,
										 const uint16_t      nextOffset, const uint32_t stride)
	{
		switch (attribute)
		{
			case MeshAttribute::POSITION:
			case MeshAttribute::NORMAL:
			case MeshAttribute::TANGENT:
			case MeshAttribute::BINORMAL:
				return 12;
			case MeshAttribute::COLOR0:
			case MeshAttribute::COLOR1:
			case MeshAttribute::COLOR2:
			case MeshAttribute::COLOR3:
				return 16;
			case MeshAttribute::TEXCOORD0:
			case MeshAttribute::TEXCOORD1:
			case MeshAttribute::TEXCOORD2:
			case MeshAttribute::TEXCOORD3:
				return nextOffset == offset ? stride - offset : nextOffset - offset;
			case MeshAttribute::UNKNOWN:
			default:
				return 0;
		};
	}

	void Submesh::AddVertexBufferView(const ::Submesh* submesh, const MeshAttribute attribute, const uint16_t offset,
									  const uint16_t   nextOffset)
	{
		const AttributeOffsets& offsets     = submesh->GetAttributes().GetAttributeOffsets();
		const uint32_t          vertexCount = submesh->GetVertexBuffer().m_elementCount;
		const uint32_t          dataSize    = GetAttributeDataSize(attribute, offset, nextOffset, offsets.m_stride);

		if (offset != 0 || attribute == MeshAttribute::POSITION)
		{
			m_vertexBufferViews[attribute] =
			{
				m_vertexBuffer->GetGPUVirtualAddress() + vertexCount * offset,
				vertexCount * dataSize,
				dataSize
			};
		}
	}

	void Submesh::CreateShaderResourceViews(const ::Submesh* submesh)
	{
		const AttributeOffsets& offsets = submesh->GetAttributes().GetAttributeOffsets();

		m_skinningHeapHandle = Dx12Context::Get().CreateHeapHandle(4);

		AddShaderResourceViews(submesh, MeshAttribute::POSITION, offsets.m_positionOffset);
		AddShaderResourceViews(submesh, MeshAttribute::NORMAL, offsets.m_normalOffset);
		AddShaderResourceViews(submesh, MeshAttribute::TANGENT, offsets.m_tangentOffset);
		AddShaderResourceViews(submesh, MeshAttribute::BINORMAL, offsets.m_bitangentOffset);
	}

	void Submesh::AddShaderResourceViews(const ::Submesh* submesh, const MeshAttribute attribute, const uint16_t offset)
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
				submesh->GetVertexBuffer().m_elementCount * 3 * offset / 12,
				submesh->GetVertexBuffer().m_elementCount * 3,
				0,
				D3D12_BUFFER_SRV_FLAG_NONE
			};

			Dx12Context::Get().GetDevice()->CreateShaderResourceView(m_vertexBuffer.Get(), &desc,
																	 m_skinningHeapHandle->GetCPUHandle(
																		 m_skinnedAttributesCount));

			m_skinnedAttributesCount++;
		}
	}

	std::shared_ptr<DescriptorHeapHandle> Submesh::GetSkinningHeapHandle()
	{
		return m_skinningHeapHandle;
	}

	uint32_t Submesh::GetSkinnedAttributeCount() const
	{
		return m_skinnedAttributesCount;
	}

	ISubmesh* Submesh::Create(const ::Submesh* submesh)
	{
		Submesh* nativeSubmesh = new Submesh();
		nativeSubmesh->Init(submesh);
		return static_cast<ISubmesh*>(nativeSubmesh);
	}

	Submesh::Submesh() {}
}

namespace Renderer
{
	extern void Deleter(ISubmesh* submesh)
	{
		if (submesh != nullptr)
		{
			delete submesh;
		}
	}
}
