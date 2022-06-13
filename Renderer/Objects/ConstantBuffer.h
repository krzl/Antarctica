#pragma once
#include "RenderSystem.h"

namespace Renderer
{
	class ConstantBuffer
	{
	public:

		void Init(const uint32_t elementCount, const uint32_t elementSize, const void* initialData);
		void Bind(ID3D12GraphicsCommandList* commandList, const uint32_t slot, const uint32_t index = 0) const;
		bool Upload() const;
		ComPtr<ID3D12Resource> GetCurrentBuffer() const;

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

		[[nodiscard]] bool IsInitialized() const
		{
			return m_elementCount != 0;
		}

	private:

		static constexpr uint8_t DIRTY_UPDATE_MASK = (1 << RenderSystem::BUFFER_COUNT) - 1;

		std::array<ComPtr<ID3D12Resource>, RenderSystem::BUFFER_COUNT> m_buffers;
		std::vector<uint8_t> m_bufferData;

		uint32_t m_elementCount      = 0;
		uint32_t m_bufferElementSize = 0;
		mutable uint8_t m_updateMask = DIRTY_UPDATE_MASK;
	};
}
