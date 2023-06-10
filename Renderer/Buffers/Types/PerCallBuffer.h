#pragma once

namespace Renderer
{
	struct PerCallBuffer
	{
		uint32_t m_instanceCount;
		uint32_t m_vertexCount;
		uint32_t m_indexCount;
		uint32_t m_padding;

		static const PerCallBuffer DEFAULT_BUFFER;
	};
}
