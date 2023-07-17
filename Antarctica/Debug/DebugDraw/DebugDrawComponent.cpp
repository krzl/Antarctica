#include "stdafx.h"
#include "DebugDrawComponent.h"

#include "Debug/DebugDrawManager.h"
#include "Managers/TimeManager.h"

BoundingBox DebugDrawComponent::GetBoundingBox() const
{
	return BoundingBox(GetWorldPosition() - Vector3D(1.0f, 1.0f, 1.0f),
					   GetWorldPosition() + Vector3D(1.0f, 1.0f, 1.0f));
}

void DebugDrawComponent::OnCreated()
{
	DebugDrawManager::GetInstance()->m_onDrawItemQueued.AddListener([this](DebugDrawManager::ElementBuilder* builder)
	{
		m_drawElements.emplace_back(
			std::make_unique<DebugDrawElement>(
				std::move(*builder)
			));
	});
}

void DebugDrawComponent::PrepareForRender(Rendering::RenderQueue& renderQueue, const Frustum& cameraFrustum,
										  std::atomic_uint32_t&  counter)
{
	const float currentTime = TimeManager::GetInstance()->GetTimeSinceStart();
	m_drawElements.erase(
		std::remove_if(m_drawElements.begin(), m_drawElements.end(),
					   [currentTime](const std::unique_ptr<DebugDrawElement>& element)
					   {
						   return element->m_despawnTime < currentTime;
					   }), m_drawElements.end());


	for (const std::unique_ptr<DebugDrawElement>& drawElement : m_drawElements)
	{
		if (cameraFrustum.Intersect(drawElement->m_boundingBox) == Frustum::IntersectTestResult::OUTSIDE)
		{
			continue;
		}

		renderQueue[counter.fetch_add(1)] = drawElement->m_cachedRenderObject.get();
	}
}

DebugDrawComponent::DebugDrawElement::DebugDrawElement(DebugDrawManager::ElementBuilder&& builder)
	: m_despawnTime(builder.m_despawnTime),
	  m_boundingBox(builder.m_boundingBox)
{
	m_material = std::make_shared<Material>(builder.m_shader);
	m_material->SetVariable<Color>("color", builder.m_color);

	m_cachedRenderObject = std::make_unique<Rendering::QueuedRenderObject>(
		Rendering::QueuedRenderObject
		{
			&m_submesh,
			&*m_material,
			0.0f,
			Rendering::PerObjectBuffer::DEFAULT_BUFFER
		}
	);

	MeshBuffer& vertexBuffer   = m_submesh.GetVertexBuffer();
	vertexBuffer.m_elementSize = sizeof(Point3D);
	vertexBuffer.m_data.resize(sizeof(Point3D) * builder.m_vertices.size());
	MeshBuffer& indexBuffer   = m_submesh.GetIndexBuffer();
	indexBuffer.m_elementSize = sizeof(uint32_t);
	indexBuffer.m_data.resize(sizeof(uint32_t) * builder.m_indices.size());

	m_submesh.SetAttributeUsage(builder.m_attributeUsage);

	memcpy(vertexBuffer.m_data.data(), builder.m_vertices.data(), builder.m_vertices.size() * sizeof(Point3D));
	memcpy(indexBuffer.m_data.data(), builder.m_indices.data(), builder.m_indices.size() * sizeof(uint32_t));
}
