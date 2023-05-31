#include "stdafx.h"
#include "RTSCamera.h"

#include "CameraScrollComponent.h"

RTSCamera::RTSCamera()
{
	m_cameraScrollComponent = AddComponent<CameraScrollComponent>();
}
