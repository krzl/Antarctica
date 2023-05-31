#pragma once

#include "Common.h"

namespace Renderer::Dx12
{
	class DescriptorHeap;

	struct DescriptorHeapHandle
	{
		friend DescriptorHeap;

		~DescriptorHeapHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

	private:

		DescriptorHeapHandle(std::shared_ptr<DescriptorHeap> heap, uint32_t id);

		std::shared_ptr<DescriptorHeap> m_heap;
		uint32_t                        m_id;
	};

	class DescriptorHeap : public std::enable_shared_from_this<DescriptorHeap>
	{
		friend DescriptorHeapHandle;
		friend class Dx12Context;

	public:

		void        Init();
		static void OnFrameStart();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle() const;
		void                        ActivateDescriptorHeap();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartHandle() const;

		bool IsFilled() const;

		std::shared_ptr<DescriptorHeapHandle> GetNextHandle();

		void FreeHandle(const DescriptorHeapHandle* descriptorHeapHandle);

		static uint32_t GetIncrementSize();

	private:

		const uint32_t HEAP_SIZE = 128;

		ComPtr<ID3D12DescriptorHeap> m_heap;
		uint32_t                     m_nextId = 0;
		std::bitset<128>             m_usedHandles;

		static uint32_t                      m_incrementSize;
		static std::weak_ptr<DescriptorHeap> m_currentlySetDescriptorHeap;
	};
}
