#pragma once

namespace Renderer
{
	struct PerObjectBuffer
	{
		Matrix4D m_transform;
		static const PerObjectBuffer DEFAULT_BUFFER;
	};

	const PerObjectBuffer PerObjectBuffer::DEFAULT_BUFFER = {
		Transform4D::identity
	};
}