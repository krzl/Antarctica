#include "stdafx.h"
#include "Camera.h"

namespace Renderer
{
	Camera::Camera()
	{
		m_camera = AddComponent<CameraComponent>();
	}
}
