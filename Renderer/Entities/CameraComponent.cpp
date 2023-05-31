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
		return GetWorldRotation().GetRotationMatrix() *
			   Transform4D::MakeTranslation(GetWorldPosition());
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
		const Matrix4D viewProj = GetPerspectiveMatrix() * GetLookAtMatrix();

		PerCameraBuffer* buffer  = m_constantBuffer.GetData<PerCameraBuffer>();
		buffer->m_viewProjMatrix = viewProj.transpose;
	}

	void CameraComponent::SetAspectRatio(const float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
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
