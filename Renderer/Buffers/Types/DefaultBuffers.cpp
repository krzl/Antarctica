#include "stdafx.h"

#include "PerCallBuffer.h"
#include "PerCameraBuffer.h"
#include "PerObjectBuffer.h"

namespace Renderer
{
	const PerCameraBuffer PerCameraBuffer::DEFAULT_BUFFER = {
		Transform4D::identity,
		Transform4D::identity
	};

	const PerObjectBuffer PerObjectBuffer::DEFAULT_BUFFER = {
		Transform4D::identity
	};

	const PerCallBuffer PerCallBuffer::DEFAULT_BUFFER = {
		0,
		0,
		0,
		0,
	};
}
