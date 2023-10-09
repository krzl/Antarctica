#include "stdafx.h"
#include "ScratchBuffer.h"

#include "Context.h"

namespace Rendering::Dx12
{
	ScratchBufferHandle ScratchBuffer::CreateHandle(const uint32_t size, const void* data, const bool isUav)
	{
		const ScratchBufferHandle handle = Allocate(size, isUav);
		if (data)
		{
			SetData(data, handle.m_bufferId, handle.m_offset, handle.m_byteSize);
		}
		return handle;
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateSRV(const uint32_t elementSize, const uint32_t elementCount, const void* data)
	{
		const ScratchBufferHandle handle = CreateHandle(elementSize * elementCount, data);
		return CreateSRV(handle, elementSize);
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateSRV(const ScratchBufferHandle& handle, const uint32_t elementSize)
	{
		std::shared_ptr<DescriptorHeapHandle> heapHandle = Dx12Context::Get().CreateHeapHandle();

		D3D12_SHADER_RESOURCE_VIEW_DESC desc
		{
			DXGI_FORMAT_UNKNOWN,
			D3D12_SRV_DIMENSION_BUFFER,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
		};
		desc.Buffer =
		{
			handle.m_offset / elementSize,
			handle.GetByteSize() / elementSize,
			elementSize,
			D3D12_BUFFER_SRV_FLAG_NONE
		};

		Dx12Context::Get().GetDevice()->CreateShaderResourceView(m_buffers[handle.m_bufferId].Get(), &desc, heapHandle->GetCPUHandle());

		return heapHandle;
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateCBV(const uint32_t size, const void* data)
	{
		const ScratchBufferHandle handle = CreateHandle(size, data);
		return CreateCBV(handle);
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateCBV(const ScratchBufferHandle& handle)
	{
		std::shared_ptr<DescriptorHeapHandle> heapHandle = Dx12Context::Get().CreateHeapHandle();

		const D3D12_CONSTANT_BUFFER_VIEW_DESC desc
		{
			GetGpuPointer(handle),
			handle.GetByteSize()
		};

		Dx12Context::Get().GetDevice()->CreateConstantBufferView(&desc, heapHandle->GetCPUHandle());

		return heapHandle;
	}

	D3D12_GPU_VIRTUAL_ADDRESS ScratchBuffer::GetGpuPointer(const ScratchBufferHandle& scratchBufferHandle) const
	{
		return m_buffers[scratchBufferHandle.m_bufferId]->GetGPUVirtualAddress() + scratchBufferHandle.m_offset;
	}

	std::shared_ptr<DescriptorHeapHandle> ScratchBuffer::CreateUAV(const ScratchBufferHandle& handle, const uint32_t elementSize)
	{
		ID3D12Device* device = Dx12Context::Get().GetDevice();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
			DXGI_FORMAT_UNKNOWN,
			D3D12_UAV_DIMENSION_BUFFER
		};
		uavDesc.Buffer = {
			handle.m_offset / elementSize,
			handle.m_byteSize / elementSize,
			elementSize,
			0,
			D3D12_BUFFER_UAV_FLAG_NONE,
		};

		std::shared_ptr<DescriptorHeapHandle> heapHandle = Dx12Context::Get().CreateHeapHandle();

		device->CreateUnorderedAccessView(m_buffers[handle.m_bufferId].Get(), nullptr, &uavDesc, heapHandle->GetCPUHandle());

		return heapHandle;
	}

	uint32_t ScratchBuffer::CreateNewBuffer(const bool bIsUav)
	{
		const uint32_t bufferId = static_cast<uint32_t>(m_buffers.size());

		ComPtr<ID3D12Resource>& buffer = m_buffers.emplace_back();

		ID3D12Device* device = Dx12Context::Get().GetDevice();

		const CD3DX12_HEAP_PROPERTIES heapProperties(bIsUav ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC bufferInfo = CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE,
			bIsUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE);

		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferInfo,
			bIsUav ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));

		std::ostringstream scratchBufferStream;
		scratchBufferStream << "Scratch Buffer " << bufferId;
		if (bIsUav)
		{
			scratchBufferStream << " (UAV)";
		}
		SetDebugName(buffer, scratchBufferStream.str().c_str());

		if (!bIsUav)
		{
			void* mappedBuffer;
			buffer->Map(0, nullptr, &mappedBuffer);
			m_mappedBuffers.emplace_back((char*) mappedBuffer);
		}
		else
		{
			m_mappedBuffers.emplace_back(nullptr);
		}

		m_bufferAllocationSizes.emplace_back(0);

		return bufferId;
	}

	static uint32_t GetAlignedStartAddress(const uint32_t bufferStart, const uint32_t size)
	{
		const uint32_t mismatch = bufferStart % size;
		return mismatch == 0 ? bufferStart : bufferStart + size - mismatch;
	}

	bool ScratchBuffer::IsUavBuffer(const uint32_t i) const
	{
		return m_mappedBuffers[i] == nullptr;
	}

	ScratchBufferHandle ScratchBuffer::Allocate(const uint32_t size, const bool bIsUav)
	{
		for (uint32_t i = 0; i < m_bufferAllocationSizes.size(); ++i)
		{
			const uint32_t startAddress = GetAlignedStartAddress(m_bufferAllocationSizes[i], size);
			if (IsUavBuffer(i) == bIsUav && startAddress + size <= BUFFER_SIZE)
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

		const uint32_t bufferId = CreateNewBuffer(bIsUav);


		return ScratchBufferHandle{
			bufferId,
			0,
			size,
			this
		};
	}

	void ScratchBuffer::SetData(const void* data, const uint32_t bufferId, const uint32_t offset, const uint32_t size) const
	{
		memcpy(m_mappedBuffers[bufferId] + offset, data, size);
	}

	uint8_t* ScratchBuffer::GetDataPtr(const ScratchBufferHandle& handle) const
	{
		return (uint8_t*) m_mappedBuffers[handle.m_bufferId] + handle.m_offset;
	}

	void ScratchBuffer::SubmitBuffers() const
	{
		for (uint32_t i = 0; i < m_buffers.size(); ++i)
		{
			const ComPtr<ID3D12Resource>& buffer = m_buffers[i];

			if (!IsUavBuffer(i))
			{
				buffer->Unmap(0, nullptr);
			}
		}
	}

	void ScratchBuffer::Reset()
	{
		for (uint32_t i = 0; i < m_bufferAllocationSizes.size(); ++i)
		{
			m_bufferAllocationSizes[i] = 0;
			if (!IsUavBuffer(i))
			{
				m_buffers[i]->Map(0, nullptr, (void**) &m_mappedBuffers[i]);
			}
		}
	}
}
