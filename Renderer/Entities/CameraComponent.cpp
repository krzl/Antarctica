#include "stdafx.h"
#include "CameraComponent.h"

#include <Buffers/Types/PerCameraBuffer.h>

namespace Renderer
{
	std::unordered_set<CameraComponent*> cameraComponents;

	void CameraComponent::OnEnabled()
	{
		cameraComponents.insert(*GetRef().Cast<CameraComponent>());
		if (!m_constantBuffer.IsInitialized())
		{
			m_constantBuffer.Init(1, sizeof(PerCameraBuffer), &PerCameraBuffer::DEFAULT_BUFFER);
		}
	}

	void CameraComponent::OnDisabled()
	{
		cameraComponents.erase(*GetRef().Cast<CameraComponent>());
	}

	Matrix4D CameraComponent::GetLookAtMatrix() const
	{
		return Inverse(Transform4D::MakeTranslation(GetWorldPosition()) * GetWorldRotation().GetRotationMatrix());
	}

	Matrix4D CameraComponent::GetPerspectiveMatrix() const
	{
		using namespace Terathon;

		const float tan   = Tan(0.5f * DegToRad(m_fov));

		const float a = (m_farZ + m_nearZ) / (m_nearZ - m_farZ);
		const float b = (2.0f * m_farZ * m_nearZ) / (m_nearZ - m_farZ);

		return Matrix4D(
			1.0f / (tan * m_aspectRatio), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / tan, 0, 0.0f,
			0.0f, 0.0f, a, b,
			0.0f, 0.0f, -1.0f, 0.0f
		);
	}

	Frustum CameraComponent::GetFrustum() const
	{
		Matrix4D inversePerspectiveMatrix = Inverse(GetPerspectiveMatrix() * GetLookAtMatrix());

		Vector4D pointFearTopLeft  = inversePerspectiveMatrix * Point3D(-1.0f, 1.0f, -1.0f);
		Vector4D pointFearTopRight = inversePerspectiveMatrix * Point3D(1.0f, 1.0f, -1.0f);
		Vector4D pointFearBotLeft  = inversePerspectiveMatrix * Point3D(-1.0f, -1.0f, -1.0f);
		Vector4D pointFearBotRight = inversePerspectiveMatrix * Point3D(1.0f, -1.0f, -1.0f);
		Vector4D pointFarTopLeft   = inversePerspectiveMatrix * Point3D(-1.0f, 1.0f, 1.0f);
		Vector4D pointFarTopRight  = inversePerspectiveMatrix * Point3D(1.0f, 1.0f, 1.0f);
		Vector4D pointFarBotLeft   = inversePerspectiveMatrix * Point3D(-1.0f, -1.0f, 1.0f);
		Vector4D pointFarBotRight  = inversePerspectiveMatrix * Point3D(1.0f, -1.0f, 1.0f);


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

	void CameraComponent::UpdateConstantBuffer()
	{
		const Matrix4D proj     = GetPerspectiveMatrix();
		const Matrix4D view     = GetLookAtMatrix();
		const Matrix4D viewProj = proj * view;

		PerCameraBuffer* buffer  = m_constantBuffer.GetData<PerCameraBuffer>();
		buffer->m_viewProjMatrix = viewProj.transpose;
	}

	void CameraComponent::SetAspectRatio(const float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
	}

	CameraComponent* CameraComponent::Get()
	{
		return *cameraComponents.begin();
	}

	std::priority_queue<CameraData> CameraComponent::GetAllCameraData()
	{
		std::priority_queue<CameraData> cameraData;

		for (CameraComponent* camera : cameraComponents)
		{
			camera->UpdateConstantBuffer();
			cameraData.emplace(CameraData
				{
					&camera->m_constantBuffer,
					camera->GetOrder()
				});
		}

		return cameraData;
	}
}
