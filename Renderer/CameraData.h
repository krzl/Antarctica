#pragma once

namespace Renderer
{
	class ConstantBuffer;
	struct CameraData
	{
		ConstantBuffer* m_constantBuffer;
		float m_order;

		friend bool operator<(const CameraData& lhs, const CameraData& rhs)
		{
			return lhs.m_order < rhs.m_order;
		}
		friend bool operator<=(const CameraData& lhs, const CameraData& rhs)
		{
			return !(rhs < lhs);
		}
		friend bool operator>(const CameraData& lhs, const CameraData& rhs)
		{
			return rhs < lhs;
		}
		friend bool operator>=(const CameraData& lhs, const CameraData& rhs)
		{
			return !(lhs < rhs);
		}
	};
}
