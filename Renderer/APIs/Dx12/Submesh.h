#pragma once

#include "Common.h"
#include "Buffers/DynamicBuffer.h"

namespace Renderer
{
	struct ScratchBufferHandle;
}

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

		void Bind(const Shader* shader, const IBuffer* skinningBuffer) const;

		const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferViewForAttribute(MeshAttribute attribute) const;

		[[nodiscard]] uint32_t GetIndexCount() const { return m_indexCount; }
		[[nodiscard]] uint32_t GetVertexCount() const { return m_vertexCount; }

		std::shared_ptr<DescriptorHeapHandle> GetSkinningHeapHandle();

		uint32_t GetSkinnedAttributeCount() const;

		static ISubmesh* Create(const ::Submesh* submesh);

	private:

		Submesh();

		void Init(const ::Submesh* submesh);

		void CreateVertexBufferViews(const ::Submesh* submesh);
		void AddVertexBufferView(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset,
								 uint16_t         nextOffset);


		void CreateShaderResourceViews(const ::Submesh* submesh);
		void AddShaderResourceViews(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset);

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
		ComPtr<ID3D12Resource>  m_indexBuffer;
		ComPtr<ID3D12Resource>  m_indexUploadBuffer;

		std::map<MeshAttribute, D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferViews;
		ComPtr<ID3D12Resource>                            m_vertexBuffer;
		ComPtr<ID3D12Resource>                            m_vertexUploadBuffer;

		std::shared_ptr<DescriptorHeapHandle> m_skinningHeapHandle;
		uint32_t                              m_skinnedAttributesCount = 0;

		uint32_t m_vertexCount = 0;
		uint32_t m_indexCount  = 0;
	};
}

namespace Renderer
{
	class ISubmesh : public Dx12::Submesh { };
}
