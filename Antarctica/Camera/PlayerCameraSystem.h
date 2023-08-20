#pragma once

#include "CameraData.h"
#include "CameraScrollComponent.h"
#include "Components/CameraComponent.h"
#include "Input/InputListener.h"
#include "Systems/System.h"


class BVH;
struct TransformComponent;

class PlayerCameraSystem : public System<TransformComponent, Rendering::CameraComponent, CameraScrollComponent, InputListenerComponent>
{
	void OnUpdateStart() override;

	void Update(uint64_t entityId, TransformComponent* transform, Rendering::CameraComponent* camera, CameraScrollComponent* cameraScroll,
				InputListenerComponent* inputListener) override;

	Matrix4D GetPerspectiveMatrix(const Rendering::CameraComponent* camera);

	Frustum GetFrustum(Rendering::CameraComponent* camera) const;

public:

	[[nodiscard]] const std::optional<Point3D>& GetCursorWorldPosition() const { return m_cursorWorldPosition; }
	[[nodiscard]] std::vector<Rendering::CameraData>& GetCameras();

	std::shared_ptr<BVH> m_terrainBvh;

private:

	float m_aspectRatio = 1.0f;

	std::optional<Point3D> m_cursorWorldPosition;

	std::vector<Rendering::CameraData> m_cameras;
};
