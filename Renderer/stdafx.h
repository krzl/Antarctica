#pragma once

#include "../Antarctica/shared_stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

#if defined(DEBUG) | defined(_DEBUG)
#include <dxgidebug.h>
#endif

#include "../ThirdParty/terathon-math/TSMath.h"
#include "../ThirdParty/terathon-math/TSMatrix4D.h"
#include "../ThirdParty/terathon-math/TSQuaternion.h"

using namespace Terathon;