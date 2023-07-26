#include "stdafx.h"
#include "SubmeshData.h"

std::unordered_map<uint32_t, AttributeOffsets> AttributeUsage::m_attributeOffsets;

AttributeUsage::operator uint32_t() const
{
	union CastHelper
	{
		AttributeUsage m_usage;
		uint32_t m_value;
	} helper;

	helper.m_usage = *this;

	return helper.m_value;
}

const AttributeOffsets& AttributeUsage::GetAttributeOffsets() const
{
	const uint32_t id = *this;

	const auto it = m_attributeOffsets.find(id);
	if (it != m_attributeOffsets.end())
	{
		return m_attributeOffsets[id];
	}

	AttributeOffsets offsets;

	uint32_t stride = 3 * 4;

	if (m_hasNormals)
	{
		offsets.m_normalOffset = stride;
		stride += 3 * 4;
	}
	if (m_hasTangents)
	{
		offsets.m_tangentOffset = stride;
		stride += 3 * 4;
	}
	if (m_hasBitangents)
	{
		offsets.m_bitangentOffset = stride;
		stride += 3 * 4;
	}
	if (m_colorChannelCount != 0)
	{
		offsets.m_colorOffset0 = m_colorChannelCount >= 1 ? stride + 4 * 4 * 0 : 0;
		offsets.m_colorOffset1 = m_colorChannelCount >= 2 ? stride + 4 * 4 * 1 : offsets.m_colorOffset0;
		offsets.m_colorOffset2 = m_colorChannelCount >= 3 ? stride + 4 * 4 * 2 : offsets.m_colorOffset1;
		offsets.m_colorOffset3 = m_colorChannelCount >= 4 ? stride + 4 * 4 * 3 : offsets.m_colorOffset2;

		stride += 4 * 4 * m_colorChannelCount;
	}
	if (m_dataSizeTexcoord0 != 0)
	{
		offsets.m_texcoordOffset0 = stride;
		offsets.m_texcoordOffset1 = stride;
		offsets.m_texcoordOffset2 = stride;
		offsets.m_texcoordOffset3 = stride;
		stride += 4 * m_dataSizeTexcoord0;
	}
	if (m_dataSizeTexcoord1 != 0)
	{
		offsets.m_texcoordOffset1 = stride;
		offsets.m_texcoordOffset2 = stride;
		offsets.m_texcoordOffset3 = stride;
		stride += 4 * m_dataSizeTexcoord1;
	}
	if (m_dataSizeTexcoord2 != 0)
	{
		offsets.m_texcoordOffset2 = stride;
		offsets.m_texcoordOffset3 = stride;
		stride += 4 * m_dataSizeTexcoord2;
	}
	if (m_dataSizeTexcoord3 != 0)
	{
		offsets.m_texcoordOffset3 = stride;
		stride += 4 * m_dataSizeTexcoord3;
	}

	offsets.m_stride = stride;

	return m_attributeOffsets[id] = offsets;
}

void Submesh::SetupNodeAttachment(const int32_t nodeId, const bool ignoreRotation) const
{
	m_attachNodeId             = nodeId;
	m_ignoreAttachmentRotation = ignoreRotation;
}
