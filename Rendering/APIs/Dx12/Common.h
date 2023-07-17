#pragma once

#include <d3d12shader.h>
#include "APIs/Dx12/d3dx12.h"

using Microsoft::WRL::ComPtr;

#define RELEASE_DX(obj) if (obj.Get() != nullptr) obj->Release();

#define STRINGIFY(x) #x

#define DXCALL(result) if (FAILED(result)) {\
std::cout << "DXError: file " STRINGIFY(__LINE__) " line " STRINGIFY(__LINE__) << std::endl;\
__debugbreak();\
}

namespace Rendering::Dx12
{
	void SetDebugName(ComPtr<ID3D12Object> object, const char* name);
	void SetDebugName(ID3D12Object* object, const char* name);
}
