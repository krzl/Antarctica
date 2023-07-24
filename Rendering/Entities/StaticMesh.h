#pragma once

#include <memory>
#include <memory>

#include "Assets/Material.h"
#include "Assets/Mesh.h"
#include "Entities/Entity.h"

namespace Rendering
{
	struct MeshComponent;

	class StaticMesh : public Entity
	{
	public:

		explicit StaticMesh(const std::shared_ptr<Mesh>& mesh);

		void SetMaterial(const std::shared_ptr<Material>& material, uint32_t index = 0);

	protected:

		void DefineArchetype(ArchetypeBuilder& builder) override;
		void SetupComponents(ComponentAccessor& accessor) override;

	private:

		std::shared_ptr<Mesh> m_mesh;
	};
}
