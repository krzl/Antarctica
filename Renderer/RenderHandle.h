#pragma once

#include "Assets/Material.h"

struct Submesh;

namespace Renderer
{
	class DynamicBuffer;
	class ConstantBuffer;

	struct RenderHandle
	{
		const ::Submesh* m_submesh;
		const Material*  m_material;
		ConstantBuffer*  m_constantBuffer;
		DynamicBuffer*   m_skinningBuffer;
		float            m_order;

		friend bool operator<(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return lhs.m_order < rhs.m_order;
		}

		friend bool operator<=(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return rhs >= lhs;
		}

		friend bool operator>(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>=(const RenderHandle& lhs, const RenderHandle& rhs)
		{
			return !(lhs < rhs);
		}

		RenderHandle(const ::Submesh& submesh, const Material& material,
					 ConstantBuffer&  constantBuffer)
			: m_submesh(&submesh),
			  m_material(&material),
			  m_constantBuffer(&constantBuffer),
			  m_skinningBuffer(nullptr),
			  m_order(material.GetOrder()) { }
	};
}
