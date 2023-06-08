#pragma once

#include "Assets/Material.h"

struct Submesh;

namespace Renderer
{
	class DynamicBuffer;
	class ConstantBuffer;

	struct QueuedRenderObject
	{
		const ::Submesh*      m_submesh;
		const Material*       m_material;
		float                 m_order;
		std::vector<uint8_t>  m_constantBuffer;
		std::vector<Matrix4D> m_boneTransforms;
		DynamicBuffer*        m_skinningBuffer;

		friend bool operator<(const QueuedRenderObject& lhs, const QueuedRenderObject& rhs)
		{
			uint64_t l, r;

			if (lhs.m_order != rhs.m_order)
			{
				l = lhs.m_order;
				r = rhs.m_order;
			}
			/*else if (lhs.m_material != rhs.m_material)
			{
				l = reinterpret_cast<uint64_t>(lhs.m_material);
				r = reinterpret_cast<uint64_t>(rhs.m_material);
			}*/
			else
			{
				l = reinterpret_cast<uint64_t>(lhs.m_submesh);
				r = reinterpret_cast<uint64_t>(rhs.m_submesh);
			}

			return l < r;
		}

		friend bool operator<=(const QueuedRenderObject& lhs, const QueuedRenderObject& rhs)
		{
			return rhs >= lhs;
		}

		friend bool operator>(const QueuedRenderObject& lhs, const QueuedRenderObject& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>=(const QueuedRenderObject& lhs, const QueuedRenderObject& rhs)
		{
			return !(lhs < rhs);
		}

		friend bool operator==(const QueuedRenderObject& lhs, const QueuedRenderObject& rhs)
		{
			return lhs.m_order == rhs.m_order && lhs.m_material == rhs.m_material && lhs.m_submesh == rhs.m_submesh;
		}

		QueuedRenderObject(const ::Submesh& submesh, const Material& material, std::vector<uint8_t>&& constantBuffer)
			: m_submesh(&submesh),
			  m_material(&material),
			  m_order(material.GetOrder()),
			  m_constantBuffer(std::move(constantBuffer)) { }
	};
}
