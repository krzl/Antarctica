#include "stdafx.h"
#include "Buffer.h"

#include "Context.h"

namespace Renderer::Dx12
{
	void Buffer::Init(const uint32_t elementCount, const uint32_t elementSize)
	{
		m_elementCount      = elementCount;
		m_bufferElementSize = elementSize;
		m_bufferSize        = ((elementSize * m_elementCount) + 255) & ~255;

		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC   bufferInfo = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);


		ID3D12Device* device = Dx12Context::Get().GetDevice();

		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
										&bufferInfo, D3D12_RESOURCE_STATE_GENERIC_READ,
										nullptr,
										IID_PPV_ARGS(&m_buffer)
		);

		heapHandle = Dx12Context::Get().CreateHeapHandle();

		D3D12_SHADER_RESOURCE_VIEW_DESC desc
		{
			DXGI_FORMAT_UNKNOWN,
			D3D12_SRV_DIMENSION_BUFFER,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
		};
		desc.Buffer =
		{
			0,
			m_elementCount,
			m_bufferElementSize,
			D3D12_BUFFER_SRV_FLAG_NONE
		};

		device->CreateShaderResourceView(m_buffer.Get(), &desc, heapHandle->GetCPUHandle());
	}

	void Buffer::InitUAV(const uint32_t elementCount, const uint32_t elementSize)
	{
		m_elementCount      = elementCount;
		m_bufferElementSize = elementSize;
		m_bufferSize        = ((elementSize * m_elementCount) + 255) & ~255;

		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC   bufferInfo = CD3DX12_RESOURCE_DESC::Buffer(
			m_bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);


		ID3D12Device* device = Dx12Context::Get().GetDevice();

		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
										&bufferInfo, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
										nullptr,
										IID_PPV_ARGS(&m_buffer)
		);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
			DXGI_FORMAT_UNKNOWN,
			D3D12_UAV_DIMENSION_BUFFER
		};
		uavDesc.Buffer = {
			0,
			elementCount,
			elementSize,
			0,
			D3D12_BUFFER_UAV_FLAG_NONE,
		};

		heapHandle = Dx12Context::Get().CreateHeapHandle();

		device->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, heapHandle->GetCPUHandle());
	}

	void Buffer::Bind(const uint32_t slot, const uint32_t index) const
	{
		ID3D12GraphicsCommandList* commandList = Dx12Context::Get().GetCommandList();
		commandList->SetGraphicsRootConstantBufferView(
			slot, m_buffer->GetGPUVirtualAddress() + index * m_bufferElementSize);
	}

	void Buffer::BindCompute(const uint32_t slot, const uint32_t index) const
	{
		Dx12Context::Get().GetCommandList()->SetComputeRootDescriptorTable(slot, heapHandle->GetGPUHandle());
	}

	void Buffer::SetData(const void* data, const uint32_t offset, uint32_t count)
	{
		if (data == nullptr)
		{
			return;
		}
		if (count == 0)
		{
			count = m_elementCount - offset;
		}
		const D3D12_RANGE range =
		{
			offset,
			offset + count * m_bufferElementSize
		};

		void* mappedMemory;
		m_buffer->Map(0, nullptr, &mappedMemory);

		memcpy(mappedMemory, data, count * m_bufferElementSize);

		m_buffer->Unmap(0, nullptr);
	}

	bool Buffer::IsInitialized() const
	{
		return m_buffer.GetAddressOf() != nullptr;
	}

	D3D12_GPU_VIRTUAL_ADDRESS Buffer::GetGPUAddress() const
	{
		return m_buffer->GetGPUVirtualAddress();
	}

	Buffer::~Buffer()
	{
		RELEASE_DX(m_buffer);
	}
}

namespace Renderer
{
	extern void Deleter(IBuffer* buffer)
	{
		if (buffer != nullptr)
		{
			delete buffer;
		}
	}
}
