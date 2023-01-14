#include "stdafx.h"
#include "SubmeshData.h"

namespace Renderer
{
	std::unordered_map<uint16_t, AttributeOffsets> AttributeUsage::m_attributeOffsets;

	std::unordered_map<const char*, std::vector<uint16_t AttributeOffsets::*>> semanticsToOffsetMap = {
		{ "POSITION", { &AttributeOffsets::m_positionOffset } },
		{ "NORMAL", { &AttributeOffsets::m_normalOffset } },
		{ "TANGENT", { &AttributeOffsets::m_tangentOffset } },
		{ "BINORMAL", { &AttributeOffsets::m_bitangentOffset } },
		{
			"COLOR",
			{
				&AttributeOffsets::m_colorOffset0,
				&AttributeOffsets::m_colorOffset1,
				&AttributeOffsets::m_colorOffset2,
				&AttributeOffsets::m_colorOffset3
			}
		},
		{
			"TEXCOORD",
			{
				&AttributeOffsets::m_texcoordOffset0,
				&AttributeOffsets::m_texcoordOffset1,
				&AttributeOffsets::m_texcoordOffset2,
				&AttributeOffsets::m_texcoordOffset3
			}
		}
	};

	uint32_t AttributeOffsets::GetOffset(const char* name, const uint32_t index) const
	{
		for (const auto& [semanticName, offsets] : semanticsToOffsetMap)
		{
			if (strncmp(name, semanticName, strnlen(semanticName, 255)) == 0)
			{
				return this->*(offsets[min(index, offsets.size() - 1)]);
			}
		}

		return 0x0;
	}

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
}
