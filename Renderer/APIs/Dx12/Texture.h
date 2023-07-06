#pragma once

#include <memory>

#include "Common.h"
#include "Assets/Texture.h"

namespace Renderer::Dx12
{
	struct DescriptorHeapHandle;

	class Texture
	{
	public:

		~Texture();

		void Bind(uint32_t index);

		static NativeTexture* Create(const std::shared_ptr<::Texture>& texture);

	private:

		Texture() = default;

		void Init(const ::Texture* texture);

		ComPtr<ID3D12Resource> m_texture;
		ComPtr<ID3D12Resource> m_textureUploadBuffer;


		bool m_isInitialized = false;

		std::shared_ptr<DescriptorHeapHandle> m_heapHandle;
	};
}

namespace Renderer
{
	class NativeTexture : public Dx12::Texture {};
}
