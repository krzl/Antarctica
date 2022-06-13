#pragma once
#include "Objects/SubmeshObject.h"
#include "Shaders/MaterialObject.h"

namespace Renderer
{
	class ConstantBuffer;
	struct RenderHandle
	{
		SubmeshObject& m_submesh;
		const MaterialObject& m_material;
		ConstantBuffer& m_constantBuffer;
		float m_order;

		friend bool operator<(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return lhs.m_order < rhs.m_order;
		}
		friend bool operator<=(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return !(rhs < lhs);
		}
		friend bool operator>(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return rhs < lhs;
		}
		friend bool operator>=(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return !(lhs < rhs);
		}

		RenderHandle(SubmeshObject& submesh, const MaterialObject& material,
					 ConstantBuffer& constantBuffer, const float order)
			: m_submesh(submesh),
			  m_material(material),
			  m_constantBuffer(constantBuffer),
			  m_order(order)
		{
		}
	};
}
