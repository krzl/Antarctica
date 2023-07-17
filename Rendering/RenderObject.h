#pragma once

#include "Assets/Material.h"
#include "Buffers/Types/PerObjectBuffer.h"

struct Submesh;

namespace Rendering
{
	class DynamicBuffer;
	class ConstantBuffer;

	struct QueuedRenderObject
	{
		const ::Submesh*      m_submesh;
		Material*             m_material;
		float                 m_order;
		PerObjectBuffer       m_perObjectBuffer;
		std::vector<Matrix4D> m_boneTransforms;
		std::optional<Rect>   m_clipRect;

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

		QueuedRenderObject() = default;

		QueuedRenderObject(const Submesh*               submesh, Material* material, const float order,
						   const PerObjectBuffer&       perObjectBuffer,
						   const std::vector<Matrix4D>& boneTransforms = std::vector<Matrix4D>(),
						   const std::optional<Rect>&   clipRect       = std::optional<Rect>())
			: m_submesh(submesh),
			  m_material(material),
			  m_order(order),
			  m_perObjectBuffer(perObjectBuffer),
			  m_boneTransforms(boneTransforms),
			  m_clipRect(clipRect) {}

		QueuedRenderObject(const QueuedRenderObject& other)
			: m_submesh(other.m_submesh),
			  m_material(other.m_material),
			  m_order(other.m_order),
			  m_perObjectBuffer(other.m_perObjectBuffer),
			  m_boneTransforms(other.m_boneTransforms) {}

		QueuedRenderObject(QueuedRenderObject&& other) noexcept
			: m_submesh(other.m_submesh),
			  m_material(other.m_material),
			  m_order(other.m_order),
			  m_perObjectBuffer(std::move(other.m_perObjectBuffer)),
			  m_boneTransforms(std::move(other.m_boneTransforms)) {}

		QueuedRenderObject& operator=(const QueuedRenderObject& other)
		{
			if (this == &other)
				return *this;
			m_submesh         = other.m_submesh;
			m_material        = other.m_material;
			m_order           = other.m_order;
			m_perObjectBuffer = other.m_perObjectBuffer;
			m_boneTransforms  = other.m_boneTransforms;
			return *this;
		}

		QueuedRenderObject& operator=(QueuedRenderObject&& other) noexcept
		{
			if (this == &other)
				return *this;
			m_submesh         = other.m_submesh;
			m_material        = other.m_material;
			m_order           = other.m_order;
			m_perObjectBuffer = std::move(other.m_perObjectBuffer);
			m_boneTransforms  = std::move(other.m_boneTransforms);
			return *this;
		}
	};
}
