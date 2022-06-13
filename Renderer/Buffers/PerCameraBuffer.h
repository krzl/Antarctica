#pragma once

namespace Renderer
{
	struct PerCameraBuffer
	{
		Matrix4D m_viewProjMatrix;
		static const PerCameraBuffer DEFAULT_BUFFER;
	};

	const PerCameraBuffer PerCameraBuffer::DEFAULT_BUFFER = {
		Transform4D::identity
	};
}
