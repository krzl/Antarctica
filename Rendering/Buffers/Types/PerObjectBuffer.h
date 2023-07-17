#pragma once

namespace Rendering
{
	struct PerObjectBuffer
	{
		Matrix4D m_transform;

		static const PerObjectBuffer DEFAULT_BUFFER;
	};
}
