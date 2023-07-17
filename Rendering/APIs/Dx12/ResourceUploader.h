#pragma once

#include "Common.h"

namespace Rendering::Dx12
{
	class ResourceUploader
	{
	public:

		static void UploadResource(const D3D12_RESOURCE_DESC& resourceDesc,
								   std::vector<D3D12_SUBRESOURCE_DATA> subresourceData,
								   D3D12_RESOURCE_STATES targetState, ComPtr<ID3D12Resource>& uploadBuffer,
								   ComPtr<ID3D12Resource>& buffer);
	};
}
