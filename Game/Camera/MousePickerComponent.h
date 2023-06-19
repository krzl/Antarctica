#pragma once

#include "GameObjects/Component.h"

namespace Renderer
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
	Ref<Renderer::CameraComponent> m_cameraComponent;
};

CREATE_CLASS(MousePickerComponent)
