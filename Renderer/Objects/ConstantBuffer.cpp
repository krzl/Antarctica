#include "stdafx.h"
#include "ConstantBuffer.h"

namespace Renderer
{
	void ConstantBuffer::Init(const uint32_t elementCount, const uint32_t elementSize,
							  const void*    initialData)
	{
		this->m_elementCount      = elementCount;
		this->m_bufferElementSize = (elementSize + 255) & ~255;
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC   bufferInfo = CD3DX12_RESOURCE_DESC::Buffer(m_bufferElementSize * elementCount);

		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT; ++i)
		{
			RenderSystem::Get().GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
																	 &bufferInfo, D3D12_RESOURCE_STATE_GENERIC_READ,
																	 nullptr,
																	 IID_PPV_ARGS(&m_buffers[i])
			);
		}

		m_bufferData.resize(elementSize * elementCount);
		for (uint32_t i = 0; i < elementCount; ++i)
		{
			memcpy(m_bufferData.data() + i * elementSize, initialData, elementSize);
		}
	}

	void ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList, const uint32_t slot, const uint32_t index) const
	{
		Upload();
		commandList->
			SetGraphicsRootConstantBufferView(slot,
											  GetCurrentBuffer()->GetGPUVirtualAddress() + index * m_bufferElementSize);
	}

	bool ConstantBuffer::Upload() const
	{
		const uint8_t currentMask = 1 << RenderSystem::Get().GetCurrentBackbufferId();
		if (m_updateMask & currentMask)
		{
			void* e;
			GetCurrentBuffer()->Map(0, nullptr, &e);

			for (uint32_t i = 0; i < m_elementCount; ++i)
			{
				memcpy(e, m_bufferData.data(), m_bufferData.size());
			}

			GetCurrentBuffer()->Unmap(0, nullptr);

			m_updateMask &= ~currentMask;

			// Check if all buffers have been updated
			return m_updateMask == 0;
		}

		return false;
	}

	ComPtr<ID3D12Resource> ConstantBuffer::GetCurrentBuffer() const
	{
		return m_buffers[RenderSystem::Get().GetCurrentBackbufferId()];
	}
}
