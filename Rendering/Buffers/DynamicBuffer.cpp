#include "stdafx.h"
#include "DynamicBuffer.h"

#include "APIs/Dx12/Buffer.h"

namespace Rendering
{
	void DynamicBuffer::Init(const uint32_t elementCount, const uint32_t elementSize)
	{
		this->m_elementCount      = elementCount;
		this->m_bufferElementSize = elementSize;

		for (uint32_t i = 0; i < Renderer::BUFFER_COUNT; ++i)
		{
			m_buffers[i] = new NativeBuffer();
			m_buffers[i]->Init(m_elementCount, m_bufferElementSize);
		}
	}

	void DynamicBuffer::InitUAV(const uint32_t elementCount, const uint32_t elementSize)
	{
		this->m_elementCount      = elementCount;
		this->m_bufferElementSize = elementSize;

		for (uint32_t i = 0; i < Renderer::BUFFER_COUNT; ++i)
		{
			m_buffers[i] = new NativeBuffer();
			m_buffers[i]->InitUAV(m_elementCount, m_bufferElementSize);
		}
	}

	NativeBuffer* DynamicBuffer::GetCurrentBuffer() const
	{
		return m_buffers[Renderer::Get().GetCurrentBackbufferId()];
	}

	DynamicBuffer::~DynamicBuffer()
	{
		for (const NativeBuffer* buffer : m_buffers)
		{
			if (buffer)
			{
				delete buffer;
			}
		}
	}
}
