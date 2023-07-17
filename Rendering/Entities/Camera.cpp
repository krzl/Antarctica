#include "stdafx.h"
#include "Camera.h"

namespace Rendering
{
	Camera::Camera()
	{
		m_camera = AddComponent<CameraComponent>();
	}
}
