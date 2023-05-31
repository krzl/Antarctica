#pragma once

#include "Entities/Camera.h"

class CameraScrollComponent;

class RTSCamera : public Renderer::Camera
{
public:
	
	RTSCamera();

private:

	Ref<CameraScrollComponent> m_cameraScrollComponent;

	DEFINE_CLASS()
};

CREATE_CLASS(RTSCamera)
