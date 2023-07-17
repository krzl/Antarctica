#include "stdafx.h"
#include "Common.h"

namespace Rendering::Dx12
{
	void SetDebugName(const ComPtr<ID3D12Object> object, const char* name)
	{
		SetDebugName(object.Get(), name);
	}

	void SetDebugName(ID3D12Object* object, const char* name)
	{
		wchar_t* wString = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wString, 4096);
		object->SetName(wString);
	}
}
