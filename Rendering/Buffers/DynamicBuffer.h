#pragma once

#include "Renderer.h"
#include "APIs/Types.h"

namespace Rendering
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

		std::array<NativeBuffer*, Renderer::BUFFER_COUNT> m_buffers = {};
	};
}
