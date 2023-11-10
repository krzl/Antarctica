#pragma once

namespace Rendering
{
	struct PerCallBuffer
	{
		uint32_t m_instanceCount;
		uint32_t m_vertexCount;
		uint32_t m_indexCount;
		uint32_t m_padding;
		uint32_t m_screenWidth;
		uint32_t m_screenHeight;

		static const PerCallBuffer DEFAULT_BUFFER;
	};
}
