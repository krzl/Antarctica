#pragma once

#include "CameraComponent.h"
#include "GameObjects/GameObject.h"

class Camera : public GameObject
{
DEFINE_CLASS()

public:

	Camera();

protected:

	Ref<CameraComponent> m_camera;
};

CREATE_CLASS(Camera)
