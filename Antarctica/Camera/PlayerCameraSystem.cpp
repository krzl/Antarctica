#include "stdafx.h"
#include "PlayerCameraSystem.h"

#include <complex>

#include "CameraData.h"
#include "Buffers/Types/PerCameraBuffer.h"
#include "Camera/CameraScrollComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"
#include "Input/InputListener.h"
#include "Input/InputQueue.h"

void PlayerCameraSystem::OnUpdateStart()
{
	m_aspectRatio = Application::Get().GetWindow().GetAspectRatio();
	m_cameras.clear();
}

void PlayerCameraSystem::Update(uint64_t entityId,
	TransformComponent*                  transform,
	Rendering::CameraComponent*          camera,
	CameraScrollComponent*               cameraScroll,
	InputListenerComponent*              inputListener)
{
	if (inputListener->m_inputQueue != nullptr)
	{
		if (inputListener->m_inputQueue->GetMouseButtonPress(InputCommand::MouseButtonId::MIDDLE))
		{
			cameraScroll->m_isDragging = true;
		}
		else
		{
			if (inputListener->m_inputQueue->GetMouseButtonRelease(InputCommand::MouseButtonId::MIDDLE))
			{
				cameraScroll->m_isDragging = false;
			}
		}

		if (cameraScroll->m_isDragging)
		{
			if (const InputCommand::MouseMoveInput* mouseMove = inputListener->m_inputQueue->GetMouseMove())
			{
				const Vector3D positionDelta = Vector3D(-mouseMove->m_deltaX * cameraScroll->m_cameraSpeed,
														mouseMove->m_deltaY * cameraScroll->m_cameraSpeed,
														0.0f);
				transform->m_localPosition += positionDelta;
			}
		}
	}

	camera->m_viewMatrix        = Inverse(Transform4D::MakeTranslation(transform->m_localPosition) * transform->m_localRotation.GetRotationMatrix());
	camera->m_perspectiveMatrix = GetPerspectiveMatrix(camera);

	camera->m_frustum = GetFrustum(camera);

	const Matrix4D viewProj = camera->m_perspectiveMatrix * camera->m_viewMatrix;

	Rendering::PerCameraBuffer buffer = Rendering::PerCameraBuffer::DEFAULT_BUFFER;
	buffer.m_viewProjMatrix           = viewProj.transpose;
	buffer.m_projMatrix               = camera->m_perspectiveMatrix.transpose;

	//TODO: add lock so m_cameras is not changed on multiple threads
	m_cameras.emplace_back(Rendering::CameraData{
		buffer,
		camera->m_order
	});

	//TODO: ECS: handle mouse picking

	//TODO: handle selection of entities
}

Matrix4D PlayerCameraSystem::GetPerspectiveMatrix(const Rendering::CameraComponent* camera)
{
	const float tan = Terathon::Tan(0.5f * DegToRad(camera->m_fov));

	const float a = (camera->m_farZ + camera->m_nearZ) / (camera->m_nearZ - camera->m_farZ);
	const float b = (2.0f * camera->m_farZ * camera->m_nearZ) / (camera->m_nearZ - camera->m_farZ);

	return Matrix4D(
		1.0f / (tan * m_aspectRatio),
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f / tan,
		0,
		0.0f,
		0.0f,
		0.0f,
		a,
		b,
		0.0f,
		0.0f,
		-1.0f,
		0.0f
	);
}

Frustum PlayerCameraSystem::GetFrustum(Rendering::CameraComponent* camera) const
{
	Matrix4D matrix = Inverse(camera->m_perspectiveMatrix * camera->m_viewMatrix);

	Vector4D pointFearTopLeft  = matrix * Point3D(-1.0f, 1.0f, -1.0f);
	Vector4D pointFearTopRight = matrix * Point3D(1.0f, 1.0f, -1.0f);
	Vector4D pointFearBotLeft  = matrix * Point3D(-1.0f, -1.0f, -1.0f);
	Vector4D pointFearBotRight = matrix * Point3D(1.0f, -1.0f, -1.0f);
	Vector4D pointFarTopLeft   = matrix * Point3D(-1.0f, 1.0f, 1.0f);
	Vector4D pointFarTopRight  = matrix * Point3D(1.0f, 1.0f, 1.0f);
	Vector4D pointFarBotLeft   = matrix * Point3D(-1.0f, -1.0f, 1.0f);
	Vector4D pointFarBotRight  = matrix * Point3D(1.0f, -1.0f, 1.0f);


	Point3D nearTopLeft  = (Point3D) (pointFearTopLeft / pointFearTopLeft.w).xyz;
	Point3D nearTopRight = (Point3D) (pointFearTopRight / pointFearTopRight.w).xyz;
	Point3D nearBotLeft  = (Point3D) (pointFearBotLeft / pointFearBotLeft.w).xyz;
	Point3D nearBotRight = (Point3D) (pointFearBotRight / pointFearBotRight.w).xyz;

	Point3D farTopLeft  = (Point3D) (pointFarTopLeft / pointFarTopLeft.w).xyz;
	Point3D farTopRight = (Point3D) (pointFarTopRight / pointFarTopRight.w).xyz;
	Point3D farBotLeft  = (Point3D) (pointFarBotLeft / pointFarBotLeft.w).xyz;
	Point3D farBotRight = (Point3D) (pointFarBotRight / pointFarBotRight.w).xyz;

	Point3D* frustumPoints[6][3] = {
		{ &nearTopRight, &nearTopLeft, &nearBotLeft }, // near plane
		{ &nearTopLeft, &farTopLeft, &farBotLeft }, // left plane
		{ &farTopRight, &nearTopRight, &nearBotRight }, // right plane
		{ &farTopRight, &farTopLeft, &nearTopLeft }, // top plane
		{ &nearBotRight, &nearBotLeft, &farBotLeft }, // bot plane
		{ &farTopLeft, &farTopRight, &farBotRight } // far plane
	};

	Frustum frustum;

	for (uint32_t i = 0; i < 6; ++i)
	{
		frustum.m_planes[i].m_direction = -Cross(*frustumPoints[i][0] - *frustumPoints[i][1],
												 *frustumPoints[i][2] - *frustumPoints[i][1]);

		frustum.m_planes[i].m_direction = Normalize(frustum.m_planes[i].m_direction);

		frustum.m_planes[i].m_distance = Dot(*frustumPoints[i][0], -frustum.m_planes[i].m_direction);
	}

	return frustum;
}

std::vector<Rendering::CameraData>& PlayerCameraSystem::GetCameras()
{
	return m_cameras;
}
