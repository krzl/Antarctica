#pragma once

namespace Renderer
{
	struct Submesh;

	class SubmeshObject
	{
	public:

		void Init();
		void Bind();

		void SetSubmesh(const Submesh* submesh)
		{
			m_submesh = submesh;
		}

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
		ComPtr<ID3D12Resource>  m_indexBuffer;
		ComPtr<ID3D12Resource>  m_indexUploadBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
		ComPtr<ID3D12Resource>   m_vertexBuffer;
		ComPtr<ID3D12Resource>   m_vertexUploadBuffer;

		uint32_t m_indexCount = 0;

	private:

		const Submesh* m_submesh       = nullptr;
		bool           m_isInitialized = false;
	};
}
