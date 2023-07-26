#include "stdafx.h"
#include "DescriptorHeap.h"

#include "Context.h"

namespace Rendering::Dx12
{
	uint32_t DescriptorHeap::m_incrementSize = 0;
	std::weak_ptr<DescriptorHeap> DescriptorHeap::m_currentlySetDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetCPUHandle(const uint32_t offset) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUStartHandle();
		handle.ptr += (m_id + offset) * m_heap->GetIncrementSize();
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetGPUHandle(const uint32_t offset) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUStartHandle();
		handle.ptr += (m_id + offset) * m_heap->GetIncrementSize();
		return handle;
	}

	DescriptorHeapHandle::DescriptorHeapHandle(std::shared_ptr<DescriptorHeap> heap, const uint32_t id) :
		m_heap(std::move(heap)),
		m_id(id) { }

	DescriptorHeapHandle::~DescriptorHeapHandle()
	{
		m_heap->FreeHandle(this);
	}

	void DescriptorHeap::Init(const D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
		uavHeapDesc.NumDescriptors             = HEAP_SIZE;
		uavHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavHeapDesc.Flags                      = flags;

		Dx12Context::Get().GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_heap));

		m_flags = flags;
		m_usedHandles.reset();
		m_firstUnusedIndex = 0;

		if (m_incrementSize == 0)
		{
			m_incrementSize = Dx12Context::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}

	void DescriptorHeap::OnFrameStart()
	{
		m_currentlySetDescriptorHeap = std::weak_ptr<DescriptorHeap>();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUStartHandle() const
	{
		return m_heap->GetCPUDescriptorHandleForHeapStart();
	}

	void DescriptorHeap::ActivateDescriptorHeap()
	{
		m_currentlySetDescriptorHeap = weak_from_this();
		Dx12Context::Get().GetCommandList()->SetDescriptorHeaps(1, { m_heap.GetAddressOf() });
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUStartHandle() const
	{
		if (m_currentlySetDescriptorHeap.expired() || m_currentlySetDescriptorHeap.lock() != shared_from_this())
		{
			const_cast<DescriptorHeap*>(this)->ActivateDescriptorHeap();
		}
		return m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	int32_t DescriptorHeap::GetNextId(const uint32_t size) const
	{
		int32_t startIndex   = -1;
		uint32_t streakCount = 0;

		for (uint32_t i = m_firstUnusedIndex; i < HEAP_SIZE; ++i)
		{
			if (!m_usedHandles[i])
			{
				if (startIndex == -1)
				{
					startIndex = i;
				}
				streakCount++;

				if (streakCount >= size)
				{
					return startIndex;
				}
			}
			else
			{
				startIndex  = -1;
				streakCount = 0;
			}
		}

		return -1;
	}

	uint32_t DescriptorHeap::GetIncrementSize()
	{
		return m_incrementSize;
	}

	std::shared_ptr<DescriptorHeapHandle> DescriptorHeap::GetNextHandle(const uint32_t size)
	{
		const int32_t nextId = GetNextId(size);

		if (nextId == -1)
		{
			return nullptr;
		}

		if (nextId == m_firstUnusedIndex)
		{
			m_firstUnusedIndex += size;
		}

		for (uint32_t i = 0; i < size; ++i)
		{
			m_usedHandles[nextId + i] = true;
		}

		return std::shared_ptr<DescriptorHeapHandle>(new DescriptorHeapHandle(shared_from_this(), nextId));
	}

	void DescriptorHeap::FreeHandle(const DescriptorHeapHandle* descriptorHeapHandle)
	{
		if (descriptorHeapHandle->m_id < m_firstUnusedIndex)
		{
			m_firstUnusedIndex = descriptorHeapHandle->m_id;
		}
		m_usedHandles[descriptorHeapHandle->m_id] = false;
	}
}
