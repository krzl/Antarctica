#pragma once

#include "MousePickerComponent.h"
#include "Entities/Camera.h"

class CameraScrollComponent;

class RTSCamera : public Rendering::Camera
{
public:
	
	RTSCamera();

private:

	Ref<CameraScrollComponent> m_cameraScrollComponent;
	Ref<MousePickerComponent>  m_mousePicker;

	DEFINE_CLASS()
};

CREATE_CLASS(RTSCamera)
