#pragma once

#include "GameObjects/Component.h"

namespace Rendering
{
	class CameraComponent;
}

class MousePickerComponent : public Component
{
protected:

	void OnCreated() override;

private:

	DEFINE_CLASS()

	DispatchHandle<>               m_inputHandle = {};
	Ref<Rendering::CameraComponent> m_cameraComponent;
};

CREATE_CLASS(MousePickerComponent)
