#include "stdafx.h"
#include "SubmeshData.h"

std::unordered_map<uint16_t, AttributeOffsets> AttributeUsage::m_attributeOffsets;

AttributeUsage::operator uint16_t() const
{
	union CastHelper
	{
		AttributeUsage m_usage;
		uint16_t       m_value;
	}                  helper;

	helper.m_usage = *this;

	return helper.m_value;
}

const AttributeOffsets& AttributeUsage::GetAttributeOffsets() const
{
	const uint16_t id = *this;
	return m_attributeOffsets[id];
}
