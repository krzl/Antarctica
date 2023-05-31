#pragma once

#include "CameraComponent.h"
#include "GameObjects/GameObject.h"

namespace Renderer
{
	class Camera : public GameObject
	{
	public:

		Camera();

		[[nodiscard]] const Ref<CameraComponent>& GetCameraComponent() const { return m_camera; }
		[[nodiscard]] Ref<CameraComponent>& GetCameraComponent() { return m_camera; }

	protected:

		Ref<CameraComponent> m_camera;

		DEFINE_CLASS()
	};

	CREATE_CLASS(Camera)
}
