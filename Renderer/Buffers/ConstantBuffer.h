#pragma once

#include "DynamicBuffer.h"
#include "RenderSystem.h"

namespace Renderer
{
	class ConstantBuffer : public DynamicBuffer
	{
	public:

		void Init(const uint32_t elementCount, const uint32_t elementSize, const void* initialData);
		bool IsOutOfDate() const;
		void UpdateCurrentBuffer();

		template<typename T>
		const T* GetData() const
		{
			return static_cast<const T*>(static_cast<const void*>(m_bufferData.data()));
		}

		template<typename T>
		T* GetData()
		{
			m_updateMask = DIRTY_UPDATE_MASK;
			return static_cast<T*>(static_cast<void*>(m_bufferData.data()));
		}

		NativeBuffer* UpdateAndGetCurrentBuffer();

	private:

		void OnBufferUploaded();

		static constexpr uint8_t DIRTY_UPDATE_MASK = (1 << RenderSystem::BUFFER_COUNT) - 1;

		std::array<NativeBuffer*, RenderSystem::BUFFER_COUNT> m_buffers = {};
		std::vector<uint8_t>                             m_bufferData;

		uint32_t m_elementCount      = 0;
		uint32_t m_bufferElementSize = 0;
		uint8_t  m_updateMask        = DIRTY_UPDATE_MASK;
	};
}
