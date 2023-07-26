#include "stdafx.h"
#include "ScratchBufferHandle.h"

#include "APIs/Dx12/ScratchBuffer.h"

namespace Rendering
{
	ScratchBufferHandle::ScratchBufferHandle(const uint32_t bufferId, const uint32_t offset, const uint32_t size,
											 Dx12::ScratchBuffer* scratchBuffer) :
		m_bufferId(bufferId),
		m_offset(offset),
		m_byteSize(size),
		m_scratchBuffer(scratchBuffer) { }
}
