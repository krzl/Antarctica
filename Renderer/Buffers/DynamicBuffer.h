#pragma once

#include "RenderSystem.h"
#include "APIs/Types.h"

namespace Renderer
{
	class DynamicBuffer
	{
	public:

		virtual ~DynamicBuffer();

		void     Init(const uint32_t elementCount, const uint32_t elementSize);
		void     InitUAV(const uint32_t elementCount, const uint32_t elementSize);
		NativeBuffer* GetCurrentBuffer() const;

		[[nodiscard]] bool IsInitialized() const
		{
			return m_elementCount != 0;
		}

	protected:

		uint32_t m_elementCount      = 0;
		uint32_t m_bufferElementSize = 0;

	private:

		std::array<NativeBuffer*, RenderSystem::BUFFER_COUNT> m_buffers = {};
	};
}
