#include "stdafx.h"
#include "DescriptorHeap.h"

#include "Context.h"

namespace Renderer::Dx12
{
	uint32_t                      DescriptorHeap::m_incrementSize = 0;
	std::weak_ptr<DescriptorHeap> DescriptorHeap::m_currentlySetDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetCPUHandle() const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUStartHandle();
		handle.ptr += m_id * m_heap->GetIncrementSize();
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetGPUHandle() const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUStartHandle();
		handle.ptr += m_id * m_heap->GetIncrementSize();
		return handle;
	}

	DescriptorHeapHandle::DescriptorHeapHandle(std::shared_ptr<DescriptorHeap> heap, const uint32_t id) :
		m_heap(std::move(heap)),
		m_id(id) { }

	DescriptorHeapHandle::~DescriptorHeapHandle()
	{
		m_heap->FreeHandle(this);
	}

	void DescriptorHeap::Init()
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
		uavHeapDesc.NumDescriptors             = HEAP_SIZE;
		uavHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		Dx12Context::Get().GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_heap));

		m_usedHandles.reset();

		if (m_incrementSize == 0)
		{
			m_incrementSize = Dx12Context::Get().GetDevice()->
												 GetDescriptorHandleIncrementSize(
													 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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

	bool DescriptorHeap::IsFilled() const
	{
		return m_usedHandles.all();
	}

	uint32_t DescriptorHeap::GetIncrementSize()
	{
		return m_incrementSize;
	}

	std::shared_ptr<DescriptorHeapHandle> DescriptorHeap::GetNextHandle()
	{
		const uint32_t id = m_nextId++;
		m_usedHandles[id] = true;
		return std::shared_ptr<DescriptorHeapHandle>(new DescriptorHeapHandle(shared_from_this(), id));
	}

	void DescriptorHeap::FreeHandle(const DescriptorHeapHandle* descriptorHeapHandle)
	{
		m_usedHandles[descriptorHeapHandle->m_id] = false;
	}
}
