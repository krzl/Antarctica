#include "stdafx.h"
#include "DebugDrawSystem.h"

#include "AssetManager.h"
#include "DebugDrawer.h"
#include "RenderObject.h"
#include "Assets/DynamicMesh.h"
#include "Components/MeshComponent.h"

#include "Entities/World.h"
#include "Managers/TimeManager.h"

void DebugDrawSystem::Init()
{
	System::Init();

	m_shader = AssetManager::GetAsset<Shader>("../Resources/Shaders/debug_draw.hlsl");

	m_attributeUsage = {
		false,
		false,
		false,
		1,
		0,
		0,
		0,
		0
	};

	DebugDrawManager::GetInstance()->m_onDrawItemQueued.AddListener([this](std::shared_ptr<DebugDrawManager::ElementBuilder> builder)
	{
		m_awaitingElements.emplace_back(builder);
	});

	World::Get()->Spawn<DebugDrawer>({});
}

void DebugDrawSystem::Update(uint64_t entityId, DebugDrawComponent* debugDraw, Rendering::MeshComponent* mesh)
{
	std::shared_ptr<DynamicMesh> dynamicMesh;
	if (mesh->m_mesh == nullptr)
	{
		mesh->m_mesh = dynamicMesh = std::make_shared<DynamicMesh>();
	}
	else
	{
		dynamicMesh = std::static_pointer_cast<DynamicMesh>(mesh->m_mesh);
	}

	std::vector<Submesh>& submeshes = dynamicMesh->GetSubmeshes();

	const float currentTime = TimeManager::GetInstance()->GetTimeSinceStart();
	//TODO: remove elements created in current frame but in not current lockstep
	//TODO: keep elements from previous locksteps with duration == 0?
	for (auto it = m_despawnTimes.begin(); it != m_despawnTimes.end();)
	{
		if (*it < currentTime)
		{
			const uint32_t index = (uint32_t) std::distance(m_despawnTimes.begin(), it);
			submeshes.erase(submeshes.begin() + index);
			mesh->m_materials.erase(mesh->m_materials.begin() + index);
			it = m_despawnTimes.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (const std::shared_ptr<DebugDrawManager::ElementBuilder>& awaitingElement : m_awaitingElements)
	{
		m_despawnTimes.emplace_back(awaitingElement->m_despawnTime);

		std::shared_ptr<Material> material        = std::make_shared<Material>(m_shader);
		material->GetShaderParams().m_isWireframe = true;
		material->SetVariable<Color>("color", awaitingElement->m_color);
		mesh->m_materials.emplace_back(material);

		Submesh& submesh = submeshes.emplace_back();

		MeshBuffer& vertexBuffer   = submesh.GetVertexBuffer();
		vertexBuffer.m_elementSize = sizeof(Point3D);
		vertexBuffer.m_data.resize(sizeof(Point3D) * awaitingElement->m_vertices.size());
		MeshBuffer& indexBuffer   = submesh.GetIndexBuffer();
		indexBuffer.m_elementSize = sizeof(uint32_t);
		indexBuffer.m_data.resize(sizeof(uint32_t) * awaitingElement->m_indices.size());

		submesh.SetAttributesUsage(m_attributeUsage);

		memcpy(vertexBuffer.m_data.data(), awaitingElement->m_vertices.data(), awaitingElement->m_vertices.size() * sizeof(Point3D));
		memcpy(indexBuffer.m_data.data(), awaitingElement->m_indices.data(), awaitingElement->m_indices.size() * sizeof(uint32_t));

		submesh.SetBoundingBox(awaitingElement->m_boundingBox);
	}
	m_awaitingElements.clear();
}

void DebugDrawSystem::OnUpdateEnd()
{
	System::OnUpdateEnd();
	m_awaitingElements.clear();
}
