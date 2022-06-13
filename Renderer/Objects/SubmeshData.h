#pragma once

namespace Renderer
{
	struct MeshBuffer
	{
		std::vector<uint8_t> m_data;
		uint32_t m_elementSize;
	};

	struct AttributeUsage
	{
		uint8_t m_hasNormals : 1;
		uint8_t m_hasTangents : 1;
		uint8_t m_hasBitangents : 1;
		uint8_t m_colorChannelCount : 2;
		uint8_t m_dataSizeTexcoord0 : 2;
		uint8_t m_dataSizeTexcoord1 : 2;
		uint8_t m_dataSizeTexcoord2 : 2;
		uint8_t m_dataSizeTexcoord3 : 2;
	};

	struct Submesh
	{
	public:

		explicit Submesh(const MeshBuffer&& vertexBuffer,
						 const MeshBuffer&& indexBuffer,
						 const AttributeUsage attributes) :
			m_vertexBuffer(vertexBuffer),
			m_indexBuffer(indexBuffer),
			m_attributes(attributes)
		{
		}

		[[nodiscard]] const MeshBuffer& GetVertexBuffer() const
		{
			return m_vertexBuffer;
		}

		[[nodiscard]] const MeshBuffer& GetIndexBuffer() const
		{
			return m_indexBuffer;
		}

		[[nodiscard]] const AttributeUsage& GetAttributes() const
		{
			return m_attributes;
		}

	private:

		MeshBuffer m_vertexBuffer;
		MeshBuffer m_indexBuffer;
		AttributeUsage m_attributes;
	};
}
