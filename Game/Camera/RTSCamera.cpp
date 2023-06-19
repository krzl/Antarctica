#include "stdafx.h"
#include "RTSCamera.h"

#include "CameraScrollComponent.h"
#include "MousePickerComponent.h"

RTSCamera::RTSCamera()
{
	m_cameraScrollComponent = AddComponent<CameraScrollComponent>();
	m_mousePicker = AddComponent<MousePickerComponent>();
}
