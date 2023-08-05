#pragma once

#include <vector>

#include "Skeleton.h"
#include "Types.h"

struct MeshBuffer
{
	std::vector<uint8_t> m_data;
	uint32_t m_elementSize;

	MeshBuffer(const std::vector<uint8_t>& data, const uint32_t elementSize) :
		m_data(data),
		m_elementSize(elementSize) {}

	MeshBuffer() = default;

	MeshBuffer(const MeshBuffer& other) :
		m_data(other.m_data),
		m_elementSize(other.m_elementSize) {}

	MeshBuffer(MeshBuffer&& other) noexcept :
		m_data(std::move(other.m_data)),
		m_elementSize(other.m_elementSize) {}

	MeshBuffer& operator=(const MeshBuffer& other)
	{
		if (this == &other)
			return *this;
		m_data        = other.m_data;
		m_elementSize = other.m_elementSize;
		return *this;
	}

	MeshBuffer& operator=(MeshBuffer&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_data        = std::move(other.m_data);
		m_elementSize = other.m_elementSize;
		return *this;
	}

	[[nodiscard]] uint32_t GetElementCount() const { return (uint32_t) m_data.size() / m_elementSize; }
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
	uint8_t m_dataSizeTexcoord0 : 3;
	uint8_t m_dataSizeTexcoord1 : 3;
	uint8_t m_dataSizeTexcoord2 : 3;
	uint8_t m_dataSizeTexcoord3 : 3;

	// ReSharper disable once CppNonExplicitConversionOperator
	operator uint32_t() const;

	[[nodiscard]] const AttributeOffsets& GetAttributeOffsets() const;

	static std::unordered_map<uint32_t, AttributeOffsets> m_attributeOffsets;
};

struct Submesh
{
	typedef std::unique_ptr<Rendering::NativeSubmesh, void(*)(Rendering::NativeSubmesh*)> NativePtr;

	Submesh() = default;

	explicit Submesh(std::string&& name, const MeshBuffer&& vertexBuffer, const MeshBuffer&& indexBuffer, const AttributeUsage attributes,
					 const BoundingBox& boundingBox) :
		m_name(std::move(name)),
		m_vertexBuffer(std::move(vertexBuffer)),
		m_indexBuffer(std::move(indexBuffer)),
		m_skeleton(),
		m_attributes(attributes),
		m_ignoreAttachmentRotation(false),
		m_boundingBox(boundingBox) { }

	[[nodiscard]] const MeshBuffer& GetVertexBuffer() const { return m_vertexBuffer; }
	[[nodiscard]] MeshBuffer& GetVertexBuffer() { return m_vertexBuffer; }

	[[nodiscard]] const MeshBuffer& GetIndexBuffer() const { return m_indexBuffer; }
	[[nodiscard]] MeshBuffer& GetIndexBuffer() { return m_indexBuffer; }

	[[nodiscard]] const AttributeUsage& GetAttributesUsage() const { return m_attributes; }
	void SetAttributesUsage(const AttributeUsage usage) { m_attributes = usage; }

	[[nodiscard]] const Skeleton& GetSkeleton() const { return m_skeleton; }
	void SetSkeleton(Skeleton&& skeleton) { m_skeleton = std::move(skeleton); }

	[[nodiscard]] Rendering::NativeSubmesh* GetNativeObject() const { return m_nativeObject.get(); }
	void SetNativeObject(Rendering::NativeSubmesh* nativePtr) const { m_nativeObject = NativePtr(nativePtr, Rendering::Deleter); }

	const std::string& GetName() const { return m_name; }

	int32_t GetAttachmentNodeId() const { return m_attachNodeId; }

	[[nodiscard]] bool GetIgnoreAttachmentRotation() const { return m_ignoreAttachmentRotation; }

	const BoundingBox& GetBoundingBox() const { return m_boundingBox; }
	void SetBoundingBox(const BoundingBox& boundingBox) { m_boundingBox = boundingBox; }

	bool IsDynamic() const { return m_isDynamic; }
	void SetDynamic() { m_isDynamic = true; }

	void SetupNodeAttachment(int32_t nodeId, bool ignoreRotation) const;

protected:

	std::string m_name;
	MeshBuffer m_vertexBuffer;
	MeshBuffer m_indexBuffer;
	Skeleton m_skeleton;
	AttributeUsage m_attributes;
	mutable int32_t m_attachNodeId = -1;
	mutable bool m_ignoreAttachmentRotation;
	bool m_isDynamic = false;

	BoundingBox m_boundingBox;

private:

	mutable NativePtr m_nativeObject = NativePtr(nullptr, Rendering::Deleter);
};
