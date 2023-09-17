#include "stdafx.h"
#include "DynamicMeshEntity.h"

#include "Archetypes/ArchetypeBuilder.h"

#include "Assets/DynamicMesh.h"

#include "Components/ColliderComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MoveableComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"

namespace Rendering
{
	DynamicMeshEntity::DynamicMeshEntity(const std::shared_ptr<DynamicMesh>& mesh) :
		m_mesh(mesh) {}

	void DynamicMeshEntity::DefineArchetype(ArchetypeBuilder& builder)
	{
		Entity::DefineArchetype(builder);

		builder.AddComponent<MeshComponent>();
		builder.AddComponent<RenderCullComponent>();
		builder.AddComponent<ColliderComponent>();
		builder.AddComponent<TransformComponent>();
		builder.AddComponent<RenderComponent>();
		builder.AddComponent<MoveableComponent>();
	}

	void DynamicMeshEntity::SetupComponents(const ComponentAccessor& accessor)
	{
		MeshComponent* meshComponent = accessor.GetComponent<MeshComponent>();
		meshComponent->m_mesh        = m_mesh;

		//TODO: NOW: is bounding box set for dynamic meshes?
		ColliderComponent* collider = accessor.GetComponent<ColliderComponent>();
		collider->m_boundingBox     = m_mesh->GetBoundingBox();

		const TransformComponent* transformComponent = accessor.GetComponent<TransformComponent>();
		RenderComponent* renderComponent             = accessor.GetComponent<RenderComponent>();
		renderComponent->m_worldTransform            = transformComponent->GetWorldTransform();

		//TODO: do it only for some dynamic meshes?
		RenderCullComponent* renderCull = accessor.GetComponent<RenderCullComponent>();
		renderCull->m_cullSubmeshes     = true;
	}

	void DynamicMeshEntity::SetMaterial(const std::shared_ptr<Material>& material, const uint32_t index)
	{
		MeshComponent* meshComponent = GetComponentAccessor().GetComponent<MeshComponent>();
		assert(index < 8);
		if (meshComponent->m_materials.size() <= index)
		{
			while (meshComponent->m_materials.size() <= index)
			{
				meshComponent->m_materials.push_back(meshComponent->m_materials.size() == index ? material : nullptr);
			}
		}
		else
		{
			meshComponent->m_materials[index] = material;
		}
	}
}
