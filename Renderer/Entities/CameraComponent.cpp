#include "stdafx.h"
#include "CameraComponent.h"

#include <Buffers/PerCameraBuffer.h>

namespace Renderer
{
	std::unordered_set<CameraComponent*> cameraComponents;

	void CameraComponent::OnEnabled()
	{
		cameraComponents.insert(*GetRef().Cast<CameraComponent>());
		if (!m_constantBuffer.IsInitialized())
		{
			m_constantBuffer.Init(1, sizeof(Renderer::PerCameraBuffer), &Renderer::PerCameraBuffer::DEFAULT_BUFFER);
		}
	}

	void CameraComponent::OnDisabled()
	{
		cameraComponents.erase(*GetRef().Cast<CameraComponent>());
	}

	Matrix4D CameraComponent::GetLookAtMatrix() const
	{
		Vector3D forward = GetWorldRotation().GetDirectionY();
		Vector3D right   = Cross(forward, Vector3D::z_unit).Normalize();
		Vector3D up      = Cross(right, forward);

		return Matrix4D(
			Vector4D(right, 0),
			Vector4D(forward, 0),
			Vector4D(up, 0),
			Vector4D(0.0f, 0.0f, 0.0f, 1.0f)
		).transpose;
	}

	Matrix4D CameraComponent::GetPerspectiveMatrix() const
	{
		using namespace Terathon;

		const float tan = Tan(0.5f * DegToRad(m_fov));

		const float range = m_farZ / (m_farZ - m_nearZ);

		return Matrix4D(
			1 / tan / m_aspectRatio, 0.0f, 0.0f, 0.0f,
			0.0f, 0, 1 / tan, 0.0f,
			0.0f, range, 0.0f, -range * m_nearZ,
			0.0f, 1.0f, 0.0f, 0.0f
		);
	}

	void CameraComponent::UpdateConstantBuffer()
	{
		const auto viewProj = GetPerspectiveMatrix() * GetLookAtMatrix();

		Renderer::PerCameraBuffer* buffer = m_constantBuffer.GetData<Renderer::PerCameraBuffer>();
		buffer->m_viewProjMatrix          = viewProj.transpose;
	}

	void CameraComponent::SetAspectRatio(const float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
	}

	std::priority_queue<Renderer::CameraData> CameraComponent::GetAllCameraData()
	{
		std::priority_queue<Renderer::CameraData> cameraData;

		for (CameraComponent* camera : cameraComponents)
		{
			camera->UpdateConstantBuffer();
			cameraData.emplace(Renderer::CameraData
				{
					&camera->m_constantBuffer,
					camera->GetOrder()
				});
		}

		return cameraData;
	}
}
