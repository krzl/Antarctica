#include "stdafx.h"
#include "ConstantBuffer.h"

#include "APIs/Dx12/Buffer.h"

namespace Renderer
{
	void ConstantBuffer::Init(const uint32_t elementCount, const uint32_t elementSize,
							  const void*    initialData)
	{
		DynamicBuffer::Init(elementCount, elementSize);

		m_bufferData.resize(elementSize * elementCount);
		for (uint32_t i = 0; i < elementCount; ++i)
		{
			memcpy(m_bufferData.data() + i * elementSize, initialData, elementSize);
		}
	}

	bool ConstantBuffer::IsOutOfDate() const
	{
		const uint8_t currentMask = 1 << RenderSystem::Get().GetCurrentBackbufferId();
		return m_updateMask & currentMask;
	}

	NativeBuffer* ConstantBuffer::UpdateAndGetCurrentBuffer()
	{
		UpdateCurrentBuffer();
		return GetCurrentBuffer();
	}

	void ConstantBuffer::OnBufferUploaded()
	{
		const uint8_t currentMask = 1 << RenderSystem::Get().GetCurrentBackbufferId();
		m_updateMask &= ~currentMask;
	}

	void ConstantBuffer::UpdateCurrentBuffer()
	{
		NativeBuffer* buffer = GetCurrentBuffer();
		if (IsOutOfDate())
		{
			buffer->SetData(m_bufferData.data());
			OnBufferUploaded();
		}
	}
}
