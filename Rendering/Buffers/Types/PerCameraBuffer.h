#pragma once

namespace Rendering
{
	struct PerCameraBuffer
	{
		Matrix4D m_viewProjMatrix;
		Matrix4D m_projMatrix;
		Matrix4D m_viewProjInvMatrix;

		static const PerCameraBuffer DEFAULT_BUFFER;
	};
}
