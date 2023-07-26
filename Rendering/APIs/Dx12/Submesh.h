#pragma once

#include "Common.h"

struct AttributeOffsets;
struct Submesh;

namespace Rendering::Dx12
{
	struct RenderObject;
	struct DescriptorHeapHandle;

	enum class MeshAttribute
	{
		POSITION,
		NORMAL,
		TANGENT,
		BINORMAL,
		COLOR0,
		COLOR1,
		COLOR2,
		COLOR3,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		UNKNOWN
	};

	MeshAttribute GetMeshAttributeFromName(const char* attributeName, uint32_t index);
	uint16_t GetAttributeOffset(MeshAttribute attribute, const AttributeOffsets& offsets);
	uint16_t GetAttributeDataSize(MeshAttribute attribute, const AttributeOffsets& offsets);

	class Submesh
	{
	public:

		virtual ~Submesh() = default;

		virtual void Update(const ::Submesh* submesh) {}
		virtual void Bind(const RenderObject& renderObject) const = 0;

		[[nodiscard]] uint32_t GetIndexCount() const { return m_indexCount; }
		[[nodiscard]] uint32_t GetVertexCount() const { return m_vertexCount; }

	protected:

		Submesh() = default;

		virtual void Init(const ::Submesh* submesh) = 0;

		uint32_t m_vertexCount = 0;
		uint32_t m_indexCount  = 0;
	};
}

namespace Rendering
{
	class NativeSubmesh : public Dx12::Submesh { };
}
