#pragma once

namespace Rendering
{
	struct PerObjectBuffer
	{
		Matrix4D m_transform;
		Matrix4D m_transformInv;

		static const PerObjectBuffer DEFAULT_BUFFER;
	};
}
