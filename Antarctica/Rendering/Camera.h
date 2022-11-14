#pragma once

#include "CameraComponent.h"
#include "GameObjects/GameObject.h"

class Camera : public GameObject
{

public:

	Camera();

protected:

	Ref<CameraComponent> m_camera;

DEFINE_CLASS()
};

CREATE_CLASS(Camera)
