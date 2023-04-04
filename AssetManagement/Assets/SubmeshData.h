#pragma once

#include "Skeleton.h"
#include "Types.h"

struct MeshBuffer
{
	std::vector<uint8_t> m_data;
	uint32_t             m_elementSize;
	uint32_t             m_elementCount;
};

struct AttributeOffsets
{
	uint16_t m_positionOffset  = 0;
	uint16_t m_normalOffset    = 0;
	uint16_t m_tangentOffset   = 0;
	uint16_t m_bitangentOffset = 0;
	uint16_t m_colorOffset0    = 0;
	uint16_t m_colorOffset1    = 0;
	uint16_t m_colorOffset2    = 0;
	uint16_t m_colorOffset3    = 0;
	uint16_t m_texcoordOffset0 = 0;
	uint16_t m_texcoordOffset1 = 0;
	uint16_t m_texcoordOffset2 = 0;
	uint16_t m_texcoordOffset3 = 0;
	uint16_t m_stride          = 0;

	uint32_t GetOffset(const char* name, uint32_t index) const;
};

struct AttributeUsage
{
	uint8_t m_hasNormals : 1;
	uint8_t m_hasTangents : 1;
	uint8_t m_hasBitangents : 1;
	uint8_t m_colorChannelCount : 3;
	uint8_t m_dataSizeTexcoord0 : 2;
	uint8_t m_dataSizeTexcoord1 : 2;
	uint8_t m_dataSizeTexcoord2 : 2;
	uint8_t m_dataSizeTexcoord3 : 2;

	// ReSharper disable once CppNonExplicitConversionOperator
	operator uint16_t() const;

	[[nodiscard]] const AttributeOffsets& GetAttributeOffsets() const;

	static std::unordered_map<uint16_t, AttributeOffsets> m_attributeOffsets;
};

struct Submesh
{
	typedef std::unique_ptr<Renderer::ISubmesh, void(*)(Renderer::ISubmesh*)> NativePtr;

public:

	explicit Submesh(const MeshBuffer&&   vertexBuffer,
					 const MeshBuffer&&   indexBuffer,
					 const AttributeUsage attributes) :
		m_vertexBuffer(vertexBuffer),
		m_indexBuffer(indexBuffer),
		m_attributes(attributes) { }

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

	[[nodiscard]] const Skeleton& GetSkeleton() const
	{
		return m_skeleton;
	}

	void SetSkeleton(Skeleton&& skeleton)
	{
		m_skeleton = std::move(skeleton);
	}

	[[nodiscard]] Renderer::ISubmesh* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::ISubmesh* nativePtr) const
	{
		m_nativeObject = NativePtr(nativePtr, Renderer::Deleter);
	}

private:

	MeshBuffer        m_vertexBuffer;
	MeshBuffer        m_indexBuffer;
	Skeleton          m_skeleton;
	AttributeUsage    m_attributes;
	mutable NativePtr m_nativeObject = NativePtr(nullptr, Renderer::Deleter);
};
