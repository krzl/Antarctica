#pragma once

#include "Common.h"

namespace Renderer::Dx12
{
	struct DescriptorHeapHandle;

	class Buffer
	{
	public:

		void Init(const uint32_t elementCount, const uint32_t elementSize);
		void InitUAV(const uint32_t elementCount, const uint32_t elementSize);
		void Bind(uint32_t slot, uint32_t index = 0) const;
		void BindCompute(uint32_t slot, uint32_t index = 0) const;
		void SetData(const void* data, uint32_t offset = 0, uint32_t count = 0);

		bool IsInitialized() const;

		[[nodiscard]] const ComPtr<ID3D12Resource>& GetBuffer() const { return m_buffer; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const;

		~Buffer();

	private:
		
		ComPtr<ID3D12Resource> m_buffer;

		uint32_t m_elementCount      = 0;
		uint32_t m_bufferElementSize = 0;
		uint32_t m_bufferSize        = 0;

		std::shared_ptr<DescriptorHeapHandle> heapHandle;
	};
}

namespace Renderer
{
	class IBuffer : public Dx12::Buffer {};
}
