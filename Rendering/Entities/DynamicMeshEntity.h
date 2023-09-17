#pragma once

#include "Assets/Material.h"
#include "Entities/Entity.h"

class DynamicMesh;

namespace Rendering
{
	struct MeshComponent;

	class DynamicMeshEntity : public Entity
	{
	public:

		explicit DynamicMeshEntity(const std::shared_ptr<DynamicMesh>& mesh);

		void SetMaterial(const std::shared_ptr<Material>& material, uint32_t index = 0);

	protected:

		void DefineArchetype(ArchetypeBuilder& builder) override;
		void SetupComponents(const ComponentAccessor& accessor) override;

	private:

		std::shared_ptr<DynamicMesh> m_mesh = nullptr;
	};
}
