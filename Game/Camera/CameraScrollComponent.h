#pragma once

#include "GameObjects/Component.h"

class CameraScrollComponent : public Component
{
	
public:

	CameraScrollComponent();
	void Tick(float deltaTime) override;
	
private:

	const float cameraSpeed = 0.02f;

	DEFINE_CLASS()
};

CREATE_CLASS(CameraScrollComponent)
