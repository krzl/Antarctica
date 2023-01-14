#include "stdafx.h"
#include "ResourceUploader.h"

#include "RenderSystem.h"

namespace Renderer
{
	void ResourceUploader::UploadResource(const D3D12_RESOURCE_DESC& resourceDesc,
										  const std::vector<D3D12_SUBRESOURCE_DATA> subresourceData,
										  const D3D12_RESOURCE_STATES targetState,
										  ComPtr<ID3D12Resource>& uploadBuffer, ComPtr<ID3D12Resource>& buffer)
	{
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		RenderSystem::Get().GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
																 D3D12_RESOURCE_STATE_COMMON,
																 nullptr, IID_PPV_ARGS(&buffer));


		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer.Get(), 0, (uint32_t) subresourceData.size());
		const CD3DX12_RESOURCE_DESC uploadBufferInfo = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		const CD3DX12_HEAP_PROPERTIES tempHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		RenderSystem::Get().GetDevice()->CreateCommittedResource(&tempHeapProperties, D3D12_HEAP_FLAG_NONE,
																 &uploadBufferInfo,
																 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
																 IID_PPV_ARGS(&uploadBuffer));

		const CD3DX12_RESOURCE_BARRIER transitionToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(),
																								   D3D12_RESOURCE_STATE_COMMON,
																								   D3D12_RESOURCE_STATE_COPY_DEST);
		RenderSystem::Get().GetCommandList()->ResourceBarrier(1, &transitionToCopyDest);

		UpdateSubresources<10>(RenderSystem::Get().GetCommandList().Get(), buffer.Get(), uploadBuffer.Get(),
							   0, 0, (uint32_t) subresourceData.size(), subresourceData.data());


		const CD3DX12_RESOURCE_BARRIER transitionToRead = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(),
																							   D3D12_RESOURCE_STATE_COPY_DEST,
																							   targetState);
		RenderSystem::Get().GetCommandList()->ResourceBarrier(1, &transitionToRead);
	}
}
