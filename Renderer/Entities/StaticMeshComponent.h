#pragma once

#include <Assets/Material.h>
#include <Assets/Mesh.h>

#include "RenderComponent.h"
#include "Components/SceneComponent.h"

namespace Renderer
{
	class StaticMeshComponent : public RenderComponent
	{
	public:

		[[nodiscard]] const std::shared_ptr<Mesh>& GetMesh() const
		{
			return m_mesh;
		}

		virtual void SetMesh(const std::shared_ptr<Mesh>& mesh);

		[[nodiscard]] std::shared_ptr<Material> GetMaterial()
		{
			return m_materials.size() > 0 ? m_materials[0] : std::shared_ptr<Material>(nullptr);
		}

		[[nodiscard]] const std::vector<std::shared_ptr<Material>>& GetMaterials() const
		{
			return m_materials;
		}

		void SetMaterial(const std::shared_ptr<Material>& material, const uint32_t index = 0)
		{
			assert(index < 8);
			if (m_materials.size() <= index)
			{
				while (m_materials.size() <= index)
				{
					m_materials.push_back(m_materials.size() == index ? material : nullptr);
				}
			}
			else
			{
				m_materials[index] = material;
			}
		}

		void SetMaterials(const std::vector<std::shared_ptr<Material>>& materials)
		{
			m_materials = materials;
		}

		BoundingBox GetBoundingBox() const override;

		float TraceRay(const BoundingBox::RayIntersectionTester& ray, float& closestDistance) const override;

		bool m_useMeshForCollision = false;

	protected:

		virtual Transform4D GetAttachedNodeTransform(int32_t nodeId, bool ignoreAttachmentRotation);
		virtual void        SetupRenderHandle(uint32_t submeshId, Material& material, QueuedRenderObject& renderObject);
		Transform4D         GetAttachmentTransform(uint32_t submeshId) override;

		void PrepareForRender(RenderQueue& renderQueue, const Frustum& cameraFrustum, std::atomic_uint32_t& counter) override;

		std::shared_ptr<Mesh>                  m_mesh;
		std::vector<std::shared_ptr<Material>> m_materials;

	private:

		std::vector<QueuedRenderObject> m_renderHandles;

		DEFINE_CLASS()
	};

	CREATE_CLASS(StaticMeshComponent)
}
