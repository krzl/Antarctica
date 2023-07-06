#include "stdafx.h"

#include "Submesh.h"

#include "Assets/SubmeshData.h"

namespace Renderer::Dx12
{
	MeshAttribute GetMeshAttributeFromName(const char* attributeName, const uint32_t index)
	{
		struct StrCmp
		{
			bool operator()(char const* a, char const* b) const
			{
				return std::strcmp(a, b) < 0;
			}
		};

		if (std::strcmp(attributeName, "TEXCOORD") == 0)
		{
			static std::vector texcoords = {
				MeshAttribute::TEXCOORD0,
				MeshAttribute::TEXCOORD1,
				MeshAttribute::TEXCOORD2,
				MeshAttribute::TEXCOORD3
			};

			return texcoords[index];
		}

		if (std::strcmp(attributeName, "COLOR") == 0)
		{
			static std::vector colors = {
				MeshAttribute::COLOR0,
				MeshAttribute::COLOR1,
				MeshAttribute::COLOR2,
				MeshAttribute::COLOR3
			};

			return colors[index];
		}

		static std::map<const char*, MeshAttribute, StrCmp> attributeMap =
		{
			{ "POSITION", MeshAttribute::POSITION },
			{ "NORMAL", MeshAttribute::NORMAL },
			{ "TANGENT", MeshAttribute::TANGENT },
			{ "BINORMAL", MeshAttribute::BINORMAL }
		};

		const auto it = attributeMap.find(attributeName);
		if (it != attributeMap.end())
		{
			return it->second;
		}

		return MeshAttribute::UNKNOWN;
	}

	static const std::vector attributeMapping =
	{
		&AttributeOffsets::m_positionOffset,
		&AttributeOffsets::m_normalOffset,
		&AttributeOffsets::m_tangentOffset,
		&AttributeOffsets::m_bitangentOffset,
		&AttributeOffsets::m_colorOffset0,
		&AttributeOffsets::m_colorOffset1,
		&AttributeOffsets::m_colorOffset2,
		&AttributeOffsets::m_colorOffset3,
		&AttributeOffsets::m_texcoordOffset0,
		&AttributeOffsets::m_texcoordOffset1,
		&AttributeOffsets::m_texcoordOffset2,
		&AttributeOffsets::m_texcoordOffset3,
		&AttributeOffsets::m_stride,
	};

	uint16_t GetAttributeOffset(const MeshAttribute attribute, const AttributeOffsets& offsets)
	{
		return offsets.*attributeMapping[(uint8_t) attribute];
	}

	uint16_t GetAttributeDataSize(const MeshAttribute attribute, const AttributeOffsets& offsets)
	{
		const uint16_t attributeOffset = GetAttributeOffset(attribute, offsets);
		uint16_t       nextOffset      = attributeOffset;

		MeshAttribute nextAttribute = MeshAttribute::POSITION;
		while (nextOffset <= attributeOffset)
		{
			nextAttribute = (MeshAttribute) ((uint8_t) nextAttribute + 1);
			nextOffset    = GetAttributeOffset(nextAttribute, offsets);
		}
		return nextOffset - attributeOffset;
	}
}

namespace Renderer
{
	extern void Deleter(NativeSubmesh* submesh)
	{
		if (submesh != nullptr)
		{
			delete submesh;
		}
	}
}
