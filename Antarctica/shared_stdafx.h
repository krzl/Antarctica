#pragma once

#include <string>
#include <array>
#include <vector>
#include <map>
#include <unordered_set>
#include <set>

#include <optional>
#include <functional>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <exception>
#include <cstdint>
#include <cassert>
#include <iostream>

#include <chrono>

#include "../ThirdParty/terathon-math/TSMath.h"
#include "../ThirdParty/terathon-math/TSMatrix4D.h"
#include "../ThirdParty/terathon-math/TSQuaternion.h"

using Terathon::Vector2D;
using Terathon::Vector3D;
using Terathon::Vector4D;
using Terathon::Point2D;
using Terathon::Point3D;
using Terathon::Matrix2D;
using Terathon::Matrix3D;
using Terathon::Matrix4D;
using Terathon::Transform4D;
using Terathon::Quaternion;

#include "../Utilities/Common/Logger.h"
#include "../Utilities/Common/Dispatcher.h"
#include "../Utilities/Common/Random.h"
#include "../Utilities/Common/Ref.h"
#include "../Utilities/Common/MathHelpers.h"
#include "../Utilities/Math/MathFunctions.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "../Renderer/d3dx12.h"

using Microsoft::WRL::ComPtr;

#if defined(DEBUG) | defined(_DEBUG)
#include <dxgidebug.h>
#endif

using MousePosition = std::pair<uint32_t, uint32_t>;