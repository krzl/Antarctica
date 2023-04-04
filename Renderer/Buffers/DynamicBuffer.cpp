#include "stdafx.h"
#include "DynamicBuffer.h"

#include "APIs/Dx12/Buffer.h"

namespace Renderer
{
	void DynamicBuffer::Init(const uint32_t elementCount, const uint32_t elementSize)
	{
		this->m_elementCount      = elementCount;
		this->m_bufferElementSize = elementSize;

		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT; ++i)
		{
			m_buffers[i] = new IBuffer();
			m_buffers[i]->Init(m_elementCount, m_bufferElementSize);
		}
	}

	void DynamicBuffer::InitUAV(const uint32_t elementCount, const uint32_t elementSize)
	{

		this->m_elementCount = elementCount;
		this->m_bufferElementSize = elementSize;

		for (uint32_t i = 0; i < RenderSystem::BUFFER_COUNT; ++i)
		{
			m_buffers[i] = new IBuffer();
			m_buffers[i]->InitUAV(m_elementCount, m_bufferElementSize);
		}
	}

	IBuffer* DynamicBuffer::GetCurrentBuffer() const
	{
		return m_buffers[RenderSystem::Get().GetCurrentBackbufferId()];
	}

	DynamicBuffer::~DynamicBuffer()
	{
		for (const IBuffer* buffer : m_buffers)
		{
			if (buffer)
			{
				delete buffer;
			}
		}
	}
}
