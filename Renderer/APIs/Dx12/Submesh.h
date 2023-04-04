#pragma once

#include <iosfwd>
#include <vector>

#include "Common.h"
#include "Buffers/DynamicBuffer.h"

struct Submesh;

namespace Renderer::Dx12
{
	struct DescriptorHeapHandle;
	class Shader;

	enum class MeshAttribute
	{
		POSITION,
		NORMAL,
		TANGENT,
		BINORMAL,
		COLOR0,
		COLOR1,
		COLOR2,
		COLOR3,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		UNKNOWN
	};

	MeshAttribute GetMeshAttributeFromName(const char* attributeName, uint32_t index);

	class Submesh
	{
	public:

		~Submesh();

		void Bind(const Shader* shader, const std::vector<DynamicBuffer>& skinningBuffers) const;

		const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferViewForAttribute(MeshAttribute attribute) const;

		[[nodiscard]] uint32_t GetIndexCount() const
		{
			return m_indexCount;
		}

		std::shared_ptr<DescriptorHeapHandle> GetAttributeHeapHandle(MeshAttribute attribute);

		static ISubmesh* Create(const ::Submesh* submesh);

	private:

		Submesh() = default;

		void Init(const ::Submesh* submesh);

		void CreateVertexBufferViews(const ::Submesh* submesh);
		void AddVertexBufferView(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset,
								 uint16_t         nextOffset);


		void CreateShaderResourceViews(const ::Submesh* submesh);
		void AddShaderResourceView(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset);

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
		ComPtr<ID3D12Resource>  m_indexBuffer;
		ComPtr<ID3D12Resource>  m_indexUploadBuffer;

		std::map<MeshAttribute, D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferViews;
		ComPtr<ID3D12Resource>                            m_vertexBuffer;
		ComPtr<ID3D12Resource>                            m_vertexUploadBuffer;

		std::map<MeshAttribute, std::shared_ptr<DescriptorHeapHandle>> m_attributeHeapHandles;

		uint32_t m_indexCount = 0;
	};
}

namespace Renderer
{
	class ISubmesh : public Dx12::Submesh { };
}
