#include "stdafx.h"
#include "ScratchBuffer.h"

#include "Context.h"

namespace Renderer::Dx12
{
	ScratchBufferHandle ScratchBuffer::CreateHandle(const uint32_t size, const void* data)
	{
		const ScratchBufferHandle handle = Allocate(size);
		if (data)
		{
			SetData(data, handle.m_bufferId, handle.m_offset, handle.m_byteSize);
		}
		return handle;
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateSRV(const uint32_t elementSize,
																   const uint32_t elementCount,
																   const void*    data)
	{
		std::shared_ptr<DescriptorHeapHandle> heapHandle = Dx12Context::Get().CreateHeapHandle();

		const ScratchBufferHandle handle = CreateHandle(elementSize * elementCount, data);

		D3D12_SHADER_RESOURCE_VIEW_DESC desc
		{
			DXGI_FORMAT_UNKNOWN,
			D3D12_SRV_DIMENSION_BUFFER,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
		};
		desc.Buffer =
		{
			handle.m_offset / elementSize,
			elementCount,
			elementSize,
			D3D12_BUFFER_SRV_FLAG_NONE
		};

		Dx12Context::Get().GetDevice()->CreateShaderResourceView(m_buffers[handle.m_bufferId].Get(), &desc,
																 heapHandle->GetCPUHandle());

		return heapHandle;
	}

	D3D12_GPU_VIRTUAL_ADDRESS ScratchBuffer::GetGpuPointer(const ScratchBufferHandle& scratchBufferHandle) const
	{
		return m_buffers[scratchBufferHandle.m_bufferId]->GetGPUVirtualAddress() + scratchBufferHandle.m_offset;
	}

	uint32_t ScratchBuffer::CreateNewBuffer()
	{
		const uint32_t bufferId = static_cast<uint32_t>(m_buffers.size());

		ComPtr<ID3D12Resource>& buffer = m_buffers.emplace_back();

		ID3D12Device* device = Dx12Context::Get().GetDevice();

		static CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		static CD3DX12_RESOURCE_DESC   bufferInfo = CD3DX12_RESOURCE_DESC::Buffer(
			BUFFER_SIZE);

		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
										&bufferInfo, D3D12_RESOURCE_STATE_GENERIC_READ,
										nullptr,
										IID_PPV_ARGS(&buffer));



		/*ComPtr<ID3D12Resource>& uploadBuffer = m_uploadBuffers.emplace_back();

		static CD3DX12_RESOURCE_DESC uploadBufferInfo = CD3DX12_RESOURCE_DESC::Buffer(
			BUFFER_SIZE);

		const CD3DX12_HEAP_PROPERTIES tempHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		Dx12Context::Get().GetDevice()->CreateCommittedResource(&tempHeapProperties, D3D12_HEAP_FLAG_NONE,
																&uploadBufferInfo,
																D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
																IID_PPV_ARGS(&uploadBuffer));*/



		std::ostringstream scratchBufferStream;
		scratchBufferStream << "Scratch Buffer " << bufferId;
		SetDebugName(buffer, scratchBufferStream.str().c_str());

		/*std::ostringstream uploadScratchBufferStream;
		uploadScratchBufferStream << "Upload Scratch Buffer " << bufferId;
		SetDebugName(uploadBuffer, uploadScratchBufferStream.str().c_str());*/

		void* mappedBuffer;
		//uploadBuffer->Map(0, nullptr, &mappedBuffer);
		buffer->Map(0, nullptr, &mappedBuffer);
		m_mappedBuffers.emplace_back((char*) mappedBuffer);

		m_bufferAllocationSizes.emplace_back(0);

		return bufferId;
	}

	static uint32_t GetAlignedStartAddress(const uint32_t bufferStart, const uint32_t size)
	{
		const uint32_t mismatch = bufferStart % size;
		return mismatch == 0 ? bufferStart : bufferStart + size - mismatch;
	}

	ScratchBufferHandle ScratchBuffer::Allocate(const uint32_t size)
	{
		for (uint32_t i = 0; i < m_bufferAllocationSizes.size(); ++i)
		{
			const uint32_t startAddress = GetAlignedStartAddress(m_bufferAllocationSizes[i], size);
			if (startAddress + size <= BUFFER_SIZE)
			{
				m_bufferAllocationSizes[i] = startAddress + size;

				return ScratchBufferHandle{
					i,
					startAddress,
					size,
					this
				};
			}
		}

		const uint32_t bufferId = CreateNewBuffer();


		return ScratchBufferHandle{
			bufferId,
			0,
			size,
			this
		};
	}

	void ScratchBuffer::SetData(const void*    data, const uint32_t bufferId, const uint32_t offset,
								const uint32_t size) const
	{
		memcpy(m_mappedBuffers[bufferId] + offset, data, size);
	}

	void ScratchBuffer::SubmitBuffers() const
	{
		for (uint32_t i = 0; i < m_buffers.size(); ++i)
		{
			const ComPtr<ID3D12Resource>& buffer = m_buffers[i];
			/*const ComPtr<ID3D12Resource>& uploadBuffer = m_uploadBuffers[i];


			const CD3DX12_RESOURCE_BARRIER transitionToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(),
																									   D3D12_RESOURCE_STATE_COMMON,
																									   D3D12_RESOURCE_STATE_COPY_DEST);
			Dx12Context::Get().GetCommandList()->ResourceBarrier(1, &transitionToCopyDest);

			uploadBuffer->Unmap(0, nullptr);

			const CD3DX12_RESOURCE_BARRIER transitionToRead = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(),
																								   D3D12_RESOURCE_STATE_COPY_DEST,
																								   D3D12_RESOURCE_STATE_GENERIC_READ);
			Dx12Context::Get().GetCommandList()->ResourceBarrier(1, &transitionToRead);*/
			buffer->Unmap(0, nullptr);
		}
	}

	void ScratchBuffer::Reset()
	{
		for (uint32_t i = 0; i < m_bufferAllocationSizes.size(); ++i)
		{
			m_bufferAllocationSizes[i] = 0;
			//m_uploadBuffers[i]->Map(0, nullptr, (void**) &m_mappedBuffers[i]);
			m_buffers[i]->Map(0, nullptr, (void**) &m_mappedBuffers[i]);
		}
	}
}
