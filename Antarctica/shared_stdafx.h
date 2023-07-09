#pragma once

#include <array>
#include <bitset>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <algorithm>
#include <functional>
#include <mutex>
#include <optional>

#include <fstream>
#include <sstream>

#include <cassert>
#include <cstdint>
#include <exception>
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

#include "../Utilities/Common/Ref.h"

#include "../Utilities/Common/Color.h"
#include "../Utilities/Common/Dispatcher.h"
#include "../Utilities/Common/Logger.h"
#include "../Utilities/Common/MathHelpers.h"
#include "../Utilities/Common/Random.h"

#include "../Utilities/Math/BoundingBox.h"
#include "../Utilities/Math/Capsule.h"
#include "../Utilities/Math/Frustum.h"
#include "../Utilities/Math/MathFunctions.h"
#include "../Utilities/Math/Plane.h"
#include "../Utilities/Math/Ray.h"
#include "../Utilities/Math/Rect.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Windows.h>

#if defined(DEBUG) | defined(_DEBUG)
#include <dxgidebug.h>
#endif

typedef std::pair<uint32_t, uint32_t> MousePosition;
