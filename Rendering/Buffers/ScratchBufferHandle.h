#pragma once

namespace Rendering::Dx12
{
	class ScratchBuffer;
}

namespace Rendering
{
	struct ScratchBufferHandle
	{
		friend class Dx12::ScratchBuffer;

		ScratchBufferHandle() :
			ScratchBufferHandle(0, 0, 0, nullptr) {}

		bool IsValid() const
		{
			return m_byteSize != 0;
		}

		[[nodiscard]] uint32_t GetByteSize() const { return m_byteSize; }

	private:

		ScratchBufferHandle(uint32_t bufferId, uint32_t offset, uint32_t size, Dx12::ScratchBuffer* scratchBuffer);

		uint32_t m_bufferId;
		uint32_t m_offset;
		uint32_t m_byteSize;

		Dx12::ScratchBuffer* m_scratchBuffer;
	};
}
