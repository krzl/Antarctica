#pragma once

#include "GameObjects/Component.h"

class CameraScrollComponent : public Component
{
	
public:

	void Tick(float deltaTime) override;
	
private:

	const float cameraSpeed = 0.02f;

	DEFINE_CLASS()
};

CREATE_CLASS(CameraScrollComponent)
