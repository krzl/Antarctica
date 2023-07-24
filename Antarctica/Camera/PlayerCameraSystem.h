#pragma once

#include "CameraData.h"
#include "CameraScrollComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"
#include "Input/InputListener.h"
#include "Systems/System.h"

class PlayerCameraSystem : public System<TransformComponent, Rendering::CameraComponent, CameraScrollComponent, InputListenerComponent>
{
	void OnUpdateStart() override;

	void Update(uint64_t            entityId,
		TransformComponent*         transform,
		Rendering::CameraComponent* camera,
		CameraScrollComponent*      cameraScroll,
		InputListenerComponent*     inputListener) override;

	Matrix4D GetPerspectiveMatrix(const Rendering::CameraComponent* camera);

	Frustum GetFrustum(Rendering::CameraComponent* camera) const;

	bool IsLockStepSystem() override { return false; }

public:

	std::vector<Rendering::CameraData>& GetCameras();

private:

	std::vector<Rendering::CameraData> m_cameras;

	float m_aspectRatio = 1.0f;
};
