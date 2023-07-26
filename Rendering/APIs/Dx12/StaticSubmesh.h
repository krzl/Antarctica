#pragma once

#include "Submesh.h"

namespace Rendering::Dx12
{
	class StaticSubmesh : public NativeSubmesh
	{
	public:

		~StaticSubmesh() override;

		void Bind(const RenderObject& renderObject) const override;

		const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferViewForAttribute(MeshAttribute attribute) const;

		std::shared_ptr<DescriptorHeapHandle> GetSkinningHeapHandle();

		uint32_t GetSkinnedAttributeCount() const;

		static NativeSubmesh* Create(const ::Submesh* submesh);

	protected:

		StaticSubmesh() = default;

		void Init(const ::Submesh* submesh) override;

	private:

		void CreateVertexBufferViews(const ::Submesh* submesh);
		void AddVertexBufferView(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset, uint16_t dataSize);

		void CreateShaderResourceViews(const ::Submesh* submesh);
		void AddShaderResourceViews(const ::Submesh* submesh, MeshAttribute attribute, uint16_t offset);

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
		ComPtr<ID3D12Resource> m_indexBuffer;
		ComPtr<ID3D12Resource> m_indexUploadBuffer;

		std::map<MeshAttribute, D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferViews;
		ComPtr<ID3D12Resource> m_vertexBuffer;
		ComPtr<ID3D12Resource> m_vertexUploadBuffer;

		std::shared_ptr<DescriptorHeapHandle> m_skinningHeapHandle;
		uint32_t m_skinnedAttributesCount = 0;
	};
}
