#include "stdafx.h"
#include "DebugDrawSystem.h"

#include <numeric>

#include "AssetManager.h"
#include "Assets/Material.h"
#include "Assets/Shader.h"
#include "Systems/TimeSystem.h"

DebugDrawSystem::DebugDrawElement::DebugDrawElement(ElementBuilder&& builder, const float despawnTime, const BoundingBox& boundingBox, const Color& color, const std::shared_ptr<Material>& material,
													const AttributeUsage& attributeUsage)
	: m_despawnTime(despawnTime),
	  m_boundingBox(boundingBox)
{
	m_cachedRenderObject = Renderer::QueuedRenderObject
	{
		&m_submesh,
		&*material,
		0.0f,
		Renderer::PerObjectBuffer::DEFAULT_BUFFER
	};

	MeshBuffer& vertexBuffer   = m_submesh.GetVertexBuffer();
	vertexBuffer.m_elementSize = sizeof(Vertex);
	vertexBuffer.m_data.resize(sizeof(Vertex) * builder.m_vertices.size());
	MeshBuffer& indexBuffer   = m_submesh.GetIndexBuffer();
	indexBuffer.m_elementSize = sizeof(uint32_t);
	indexBuffer.m_data.resize(sizeof(uint32_t) * builder.m_indices.size());

	m_submesh.SetAttributeUsage(attributeUsage);

	memcpy(vertexBuffer.m_data.data(), builder.m_vertices.data(), builder.m_vertices.size() * sizeof(Point3D));

	for (uint32_t i = 0; i < builder.m_vertices.size(); ++i)
	{
		memcpy(&vertexBuffer.m_data[builder.m_vertices.size() * 12 + i * sizeof(Color)], &color, sizeof(Color));
	}

	memcpy(indexBuffer.m_data.data(), builder.m_indices.data(), builder.m_indices.size() * sizeof(uint32_t));
}

void DebugDrawSystem::Init()
{
	m_shader   = AssetManager::GetAsset<Shader>("../Resources/Shaders/debug_draw.hlsl");
	m_material = std::make_shared<Material>(m_shader);

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
}

void DebugDrawSystem::Update()
{
	const float currentTime = TimeSystem::GetInstance()->GetTimeSinceStart();
	m_drawElements.erase(
		std::remove_if(m_drawElements.begin(), m_drawElements.end(),
					   [currentTime](const std::unique_ptr<DebugDrawElement>& element)
					   {
						   return element->m_despawnTime <= currentTime;
					   }), m_drawElements.end());
}

static BoundingBox GetBoundingBox(const std::vector<Point3D>& pointsList)
{
	BoundingBox boundingBox(pointsList[0], pointsList[0]);
	for (uint32_t i = 1; i < pointsList.size(); ++i)
	{
		boundingBox.Append(pointsList[i]);
	}

	return boundingBox;
}

static std::vector<Point3D> GeneratePointInCircle(uint32_t pointCount, const Point3D center, const Vector3D& direction,
												  float    radius)
{
	std::vector<Point3D> points(pointCount);

	float angleIncrement = 2.0f * Terathon::Math::pi / pointCount;

	Vector3D a;

	if (std::fabs(direction.x) > 0.1f)
	{
		a.x = direction.y;
		a.y = -direction.x;
		a.z = 0.0f;
	}
	else
	{
		a.x = 0.0f;
		a.y = direction.z;
		a.z = -direction.y;
	}

	a.Normalize();

	Vector3D b(direction.y * a.z - direction.z * a.y,
			   direction.z * a.x - direction.x * a.z,
			   direction.x * a.y - direction.y * a.x);


	for (uint32_t i = 0; i < pointCount; ++i)
	{
		const float angle    = i * angleIncrement;
		const float cosAngle = Terathon::Cos(angle);
		const float sinAngle = Terathon::Sin(angle);
		const float x        = center.x + radius * (cosAngle * a.x + sinAngle * b.x);
		const float y        = center.y + radius * (cosAngle * a.y + sinAngle * b.y);
		const float z        = center.z + radius * (cosAngle * a.z + sinAngle * b.z);
		points[i]            = Point3D(x, y, z);
	};

	return points;
}

void DebugDrawSystem::DrawBox(Point3D center, Quaternion rotation, float x, float y, float z, float duration,
							  Color   color)
{
	const Vector3D xVec = rotation.GetDirectionX() * x / 2.0f;
	const Vector3D yVec = rotation.GetDirectionY() * y / 2.0f;
	const Vector3D zVec = rotation.GetDirectionZ() * z / 2.0f;

	Point3D lowerCenter = center - zVec;

	std::vector<Point3D> vertices(8);
	vertices[0] = lowerCenter - xVec - yVec;
	vertices[1] = lowerCenter - xVec + yVec;
	vertices[2] = lowerCenter + xVec + yVec;
	vertices[3] = lowerCenter + xVec - yVec;
	vertices[4] = vertices[0] + zVec * 2.0f;
	vertices[5] = vertices[1] + zVec * 2.0f;
	vertices[6] = vertices[2] + zVec * 2.0f;
	vertices[7] = vertices[3] + zVec * 2.0f;

	ElementBuilder builder = ReserveLines(12, SEGMENTS_PER_LINE);

	AppendLine(vertices[0], vertices[1], builder);
	AppendLine(vertices[1], vertices[2], builder);
	AppendLine(vertices[2], vertices[3], builder);
	AppendLine(vertices[3], vertices[0], builder);

	AppendLine(vertices[4], vertices[5], builder);
	AppendLine(vertices[5], vertices[6], builder);
	AppendLine(vertices[6], vertices[7], builder);
	AppendLine(vertices[7], vertices[4], builder);

	AppendLine(vertices[0], vertices[4], builder);
	AppendLine(vertices[1], vertices[5], builder);
	AppendLine(vertices[2], vertices[6], builder);
	AppendLine(vertices[3], vertices[7], builder);


	m_drawElements.emplace_back(
		std::make_unique<DebugDrawElement>(
			std::move(builder),
			TimeSystem::GetInstance()->GetTimeSinceStart() + duration,
			GetBoundingBox(vertices),
			color,
			m_material,
			m_attributeUsage)
	);
}


void DebugDrawSystem::DrawCylinder(Point3D center, Quaternion rotation, float height, float width, float duration,
								   Color   color, uint32_t    segments)
{
	const Vector3D up         = rotation.GetDirectionZ();
	const float    halfHeight = height / 2.0f;

	const Point3D start = center - up * halfHeight;
	const Point3D end   = center + up * halfHeight;

	const Vector3D offset    = end - start;
	const Vector3D direction = Normalize(offset);

	std::vector<Point3D> lowerCircle = GeneratePointInCircle(segments, start, direction, width);
	std::vector<Point3D> upperCircle(lowerCircle.size());
	for (uint32_t i = 0; i < lowerCircle.size(); ++i)
	{
		upperCircle[i] = lowerCircle[i] + offset;
	}

	ElementBuilder builder = ReserveLines(segments * 3, SEGMENTS_PER_LINE);

	// bottom cap
	for (uint32_t i = 0; i < segments; ++i)
	{
		AppendLine(lowerCircle[i], lowerCircle[(i + 1) % segments], builder);
	}

	// top cap
	for (uint32_t i = 0; i < segments; ++i)
	{
		AppendLine(upperCircle[i], upperCircle[(i + 1) % segments], builder);
	}

	// sides
	for (uint32_t i = 0; i < segments; ++i)
	{
		AppendLine(lowerCircle[i], upperCircle[i], builder);
	}

	BoundingBox bbLower = GetBoundingBox(lowerCircle);
	BoundingBox bbUpper = GetBoundingBox(upperCircle);
	bbLower.Append(bbUpper);


	m_drawElements.emplace_back(
		std::make_unique<DebugDrawElement>(
			std::move(builder),
			TimeSystem::GetInstance()->GetTimeSinceStart() + duration,
			bbLower,
			color,
			m_material,
			m_attributeUsage)
	);
}

void DebugDrawSystem::DrawLine(const Point3D  start, const Point3D end, const float width, const float duration, const Color color,
							   const uint32_t segments)
{
	ElementBuilder builder = ReserveLines(1, segments);
	AppendLine(start, end, builder, segments, width);

	BoundingBox boundingBox = GetBoundingBox(builder.m_vertices);


	m_drawElements.emplace_back(
		std::make_unique<DebugDrawElement>(
			std::move(builder),
			TimeSystem::GetInstance()->GetTimeSinceStart() + duration,
			boundingBox,
			color,
			m_material,
			m_attributeUsage)
	);
}

void DebugDrawSystem::DrawSphere(const Point3D center, const float radius, const float duration, const Color color, const uint32_t segments)
{
	std::vector<Point3D> vertices((segments - 2) * segments + 2);

	vertices[0]                   = center + Vector3D(0.0f, 0.0f, radius);
	vertices[vertices.size() - 1] = center - Vector3D(0.0f, 0.0f, radius);

	ElementBuilder builder = ReserveLines(2 * segments * segments - 3 * segments, SEGMENTS_PER_LINE);

	for (uint32_t i = 0; i < segments - 2; ++i)
	{
		Point3D subCircleCenter = LerpClamped(vertices[vertices.size() - 1], vertices[0], (float) (i + 1) / (segments - 1));

		const float sphereCapHeight = std::abs(center.z - subCircleCenter.z);
		const float sphereCapRadius = Terathon::Sqrt(radius * radius - sphereCapHeight * sphereCapHeight);

		std::vector<Point3D> points = GeneratePointInCircle(segments, subCircleCenter, Vector3D::z_unit, sphereCapRadius);

		memcpy(&vertices[i * segments + 1], points.data(), sizeof(Point3D) * segments);

		for (uint32_t j = 0; j < segments; ++j)
		{
			AppendLine(points[j], points[(j + 1) % segments], builder);

			if (i != 0)
			{
				AppendLine(points[j], vertices[(i - 1) * segments + 1 + j], builder);
			}
		}
	}

	//top and bottom caps
	for (uint32_t i = 0; i < segments; ++i)
	{
		AppendLine(vertices[vertices.size() - 1], vertices[i + 1], builder);
		AppendLine(vertices[0], vertices[vertices.size() - 2 - i], builder);
	}


	m_drawElements.emplace_back(
		std::make_unique<DebugDrawElement>(
			std::move(builder),
			TimeSystem::GetInstance()->GetTimeSinceStart() + duration,
			BoundingBox(center - Vector3D(radius, radius, radius), center + Vector3D(radius, radius, radius)),
			color,
			m_material,
			m_attributeUsage)
	);
}

void DebugDrawSystem::DrawTriangles(std::vector<Point3D>&& pointsList, const float duration, const Color color)
{
	ElementBuilder builder;

	builder.m_vertices = std::move(pointsList);

	builder.m_indices.resize(builder.m_vertices.size());
	std::iota(builder.m_indices.begin(), builder.m_indices.end(), 0);

	BoundingBox boundingBox = GetBoundingBox(builder.m_vertices);


	m_drawElements.emplace_back(
		std::make_unique<DebugDrawElement>(
			std::move(builder),
			TimeSystem::GetInstance()->GetTimeSinceStart() + duration,
			boundingBox,
			color,
			m_material,
			m_attributeUsage)
	);
}

DebugDrawSystem::ElementBuilder DebugDrawSystem::ReserveLines(const uint32_t lineCount, const uint32_t segmentCount)
{
	return ElementBuilder
	{
		0,
		0,
		std::vector<Point3D>(lineCount * segmentCount * 2),
		std::vector<uint32_t>(lineCount * 3 * (4 * segmentCount - 4))
	};
}

void DebugDrawSystem::AppendLine(Point3D         start, Point3D end,
								 ElementBuilder& drawElement,
								 uint32_t        segmentCount, float width) const
{
	uint32_t& currentVertex = drawElement.m_currentVertex;
	uint32_t& currentIndex  = drawElement.m_currentIndex;

	const Vector3D offset    = end - start;
	const Vector3D direction = Normalize(offset);

	std::vector<Point3D> lowerCircle = GeneratePointInCircle(segmentCount, start, direction, width);
	std::vector<Point3D> upperCircle(lowerCircle.size());

	for (uint32_t i = 0; i < lowerCircle.size(); ++i)
	{
		upperCircle[i] = lowerCircle[i] + offset;
	}

	// top and bottom cap
	for (uint32_t i = 0; i < 2 * segmentCount - 4; ++i)
	{
		const bool isTop                      = i >= segmentCount - 2;
		drawElement.m_indices[currentIndex++] = currentVertex + (isTop ? segmentCount : 0);
		drawElement.m_indices[currentIndex++] = currentVertex + i + 2 + (isTop ? 2 : 0);
		drawElement.m_indices[currentIndex++] = currentVertex + i + 1 + (isTop ? 2 : 0);
	}

	// sides
	for (uint32_t i = 0; i < segmentCount; ++i)
	{
		const uint32_t j                      = (i + 1) % segmentCount;
		drawElement.m_indices[currentIndex++] = currentVertex + i;
		drawElement.m_indices[currentIndex++] = currentVertex + segmentCount + i;
		drawElement.m_indices[currentIndex++] = currentVertex + segmentCount + j;

		drawElement.m_indices[currentIndex++] = currentVertex + segmentCount + j;
		drawElement.m_indices[currentIndex++] = currentVertex + j;
		drawElement.m_indices[currentIndex++] = currentVertex + i;
	}

	memcpy(&drawElement.m_vertices[currentVertex], lowerCircle.data(), lowerCircle.size() * sizeof(Point3D));
	memcpy(&drawElement.m_vertices[currentVertex + segmentCount], upperCircle.data(),
		   upperCircle.size() * sizeof(Point3D));

	currentVertex += 2 * segmentCount;
}

void DebugDrawSystem::Render(Renderer::RenderQueue& renderQueue, const Frustum& cameraFrustum, std::atomic_uint32_t& counter) const
{
	for (const std::unique_ptr<DebugDrawElement>& drawElement : m_drawElements)
	{
		if (cameraFrustum.Intersect(drawElement->m_boundingBox) == Frustum::IntersectTestResult::OUTSIDE)
		{
			continue;
		}

		renderQueue[counter.fetch_add(1)] = &drawElement->m_cachedRenderObject;
	}
}
