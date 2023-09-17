#pragma once

#include "Entities/Camera.h"

struct CameraScrollComponent;

class RTSCamera : public Rendering::Camera
{
protected:

	void DefineArchetype(ArchetypeBuilder& builder) override;
};
