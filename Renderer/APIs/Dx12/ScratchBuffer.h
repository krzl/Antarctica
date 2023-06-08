#pragma once

#include "Common.h"
#include "DescriptorHeap.h"
#include "Buffers/ScratchBufferHandle.h"

namespace Renderer::Dx12
{
	class ScratchBuffer
	{
		friend class Dx12Context;

	public:

		ScratchBufferHandle CreateHandle(uint32_t size, const void* data = nullptr);

		std::shared_ptr<DescriptorHeapHandle> CreateSRV(uint32_t    elementSize, uint32_t elementCount = 1,
														const void* data                               = nullptr);

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuPointer(const ScratchBufferHandle& scratchBufferHandle) const;

	private:

		uint32_t            CreateNewBuffer();
		ScratchBufferHandle Allocate(uint32_t size);
		void                SetData(const void* data, uint32_t bufferId, uint32_t offset, uint32_t size) const;

		void SubmitBuffers() const;

		void Reset();

		static constexpr uint32_t BUFFER_SIZE = 4 * 1024 * 1024;

		std::vector<ComPtr<ID3D12Resource>> m_buffers;
		std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
		std::vector<char*>                  m_mappedBuffers;
		std::vector<uint32_t>               m_bufferAllocationSizes;
	};
}
