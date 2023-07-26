#pragma once

#include "Common.h"

namespace Rendering::Dx12
{
	class DescriptorHeap;

	struct DescriptorHeapHandle
	{
		friend DescriptorHeap;

		DescriptorHeapHandle(const DescriptorHeapHandle&)            = delete;
		DescriptorHeapHandle& operator=(const DescriptorHeapHandle&) = delete;

		~DescriptorHeapHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t offset = 0) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t offset = 0) const;

	private:

		DescriptorHeapHandle(std::shared_ptr<DescriptorHeap> heap, uint32_t id);

		std::shared_ptr<DescriptorHeap> m_heap;
		uint32_t m_id;
	};

	class DescriptorHeap : public std::enable_shared_from_this<DescriptorHeap>
	{
		friend DescriptorHeapHandle;
		friend class Dx12Context;

	public:

		void Init(D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		static void OnFrameStart();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle() const;
		void ActivateDescriptorHeap();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartHandle() const;

		int32_t GetNextId(uint32_t size) const;
		std::shared_ptr<DescriptorHeapHandle> GetNextHandle(uint32_t size);

		void FreeHandle(const DescriptorHeapHandle* descriptorHeapHandle);

		static uint32_t GetIncrementSize();

	private:

		const uint32_t HEAP_SIZE = 65536;

		ComPtr<ID3D12DescriptorHeap> m_heap;
		std::bitset<65536> m_usedHandles;

		mutable uint32_t m_firstUnusedIndex = 0;

		D3D12_DESCRIPTOR_HEAP_FLAGS m_flags;

		static uint32_t m_incrementSize;
		static std::weak_ptr<DescriptorHeap> m_currentlySetDescriptorHeap;
	};
}
