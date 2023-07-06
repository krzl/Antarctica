#pragma once

namespace Renderer
{
	struct PerCameraBuffer
	{
		Matrix4D m_viewProjMatrix;
		Matrix4D m_projMatrix;

		static const PerCameraBuffer DEFAULT_BUFFER;
	};
}
