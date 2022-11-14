#pragma once

namespace Renderer
{
	class ResourceUploader
	{
	public:

		static void UploadResource(D3D12_RESOURCE_DESC& resourceDesc,
								   std::vector<D3D12_SUBRESOURCE_DATA> subresourceData,
								   D3D12_RESOURCE_STATES targetState, ComPtr<ID3D12Resource>& uploadBuffer,
								   ComPtr<ID3D12Resource>& buffer);
	};
}