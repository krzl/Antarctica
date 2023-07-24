#pragma once
#include "Buffers/Types/PerCameraBuffer.h"

namespace Rendering
{
	class ConstantBuffer;

	struct CameraData
	{
		PerCameraBuffer m_constantBuffer;
		float           m_order;

		friend bool operator<(const CameraData& lhs, const CameraData& rhs)
		{
			return lhs.m_order < rhs.m_order;
		}

		friend bool operator<=(const CameraData& lhs, const CameraData& rhs)
		{
			return lhs <= rhs;
		}

		friend bool operator>(const CameraData& lhs, const CameraData& rhs)
		{
			return lhs > rhs;
		}

		friend bool operator>=(const CameraData& lhs, const CameraData& rhs)
		{
			return lhs >= rhs;
		}
	};
}
