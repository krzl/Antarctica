#include "stdafx.h"
#include "PerCameraBuffer.h"
#include "PerObjectBuffer.h"

namespace Renderer
{
	const PerCameraBuffer Renderer::PerCameraBuffer::DEFAULT_BUFFER = {
		Transform4D::identity
	};

	const PerObjectBuffer Renderer::PerObjectBuffer::DEFAULT_BUFFER = {
		Transform4D::identity
	};
}
