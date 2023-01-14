#pragma once

class Texture;

namespace Renderer
{
	class TextureObject
	{
		friend class Texture;

	public:

		void Init();

		void Bind(uint32_t index);

	private:

		ComPtr<ID3D12Resource> m_texture;
		ComPtr<ID3D12Resource> m_textureUploadBuffer;

		const uint8_t* m_data     = nullptr;
		int32_t        m_width    = 0;
		int32_t        m_height   = 0;
		int32_t        m_channels = 0;

		uint32_t m_id            = 0xFFFFFFFF;
		bool     m_isInitialized = false;



		static uint32_t                     m_counter;
		static ComPtr<ID3D12DescriptorHeap> m_heap;

		static void InitHeap();
	};
}
