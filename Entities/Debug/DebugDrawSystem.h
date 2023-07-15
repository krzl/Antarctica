#pragma once

#include "Assets/DynamicSubmesh.h"
#include "Assets/SubmeshData.h"
#include "Systems/System.h"

class Shader;
class Material;

namespace Renderer
{
	struct QueuedRenderObject;
	class RenderQueue;
};

class DebugDrawSystem : public System
{
	REGISTER_SYSTEM(DebugDrawSystem);

	friend class DebugDrawComponent;

	struct ElementBuilder
	{
		uint32_t              m_currentVertex = 0;
		uint32_t              m_currentIndex  = 0;
		std::vector<Point3D>  m_vertices;
		std::vector<uint32_t> m_indices;

		float                   m_despawnTime = 0.0f;
		BoundingBox             m_boundingBox;
		Color                   m_color;
		std::shared_ptr<Shader> m_shader;
		AttributeUsage          m_attributeUsage;
	};

protected:

	void Init() override;

public:

	void DrawLine(Point3D start, Point3D end, float width, float duration = 0.0f, Color color = Color::red, uint32_t segments = 8);
	void DrawTriangles(std::vector<Point3D>&& pointsList, float duration = 0.0f, Color color = Color::red);
	void DrawSphere(Point3D center, float radius, float duration = 0.0f, Color color = Color::red, uint32_t segments = 12);
	void DrawCylinder(Point3D center, Quaternion rotation, float height, float width, float duration = 0.0f, Color color = Color::red, uint32_t segments = 8);
	void DrawBox(Point3D center, Quaternion rotation, float x, float y, float z, float duration = 0.0f, Color color = Color::red);

private:

	static constexpr uint32_t SEGMENTS_PER_LINE = 6;
	static constexpr float    LINE_WIDTH        = 0.03f;

	Dispatcher<ElementBuilder*> m_onDrawItemQueued;

	static ElementBuilder ReserveLines(uint32_t lineCount, const uint32_t segmentCount);

	void AppendLine(Point3D start, Point3D end, ElementBuilder& drawElement, uint32_t segmentCount = SEGMENTS_PER_LINE, float width = LINE_WIDTH) const;

	std::shared_ptr<Shader> m_shader;

	AttributeUsage m_attributeUsage;
};
