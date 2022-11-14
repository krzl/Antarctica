#pragma once

#include <CameraData.h>
#include <Objects/ConstantBuffer.h>

#include "Components/SceneComponent.h"

class CameraComponent : public SceneComponent
{
public:

	[[nodiscard]] float GetFOV() const
	{
		return m_fov;
	}

	void SetFOV(const float fov)
	{
		m_fov = fov;
	}

	[[nodiscard]] float GetFarZ() const
	{
		return m_farZ;
	}

	void SetFarZ(const float farZ)
	{
		m_farZ = farZ;
	}

	[[nodiscard]] float GetNearZ() const
	{
		return m_nearZ;
	}

	void SetNearZ(const float nearZ)
	{
		m_nearZ = nearZ;
	}

	[[nodiscard]] float GetOrder() const
	{
		return m_order;
	}

	void SetOrder(const float order)
	{
		m_order = order;
	}

	static std::set<Renderer::CameraData> GetAllCameraData();

protected:

	virtual void OnEnabled() override;
	virtual void OnDisabled() override;

	[[nodiscard]] Matrix4D GetLookAtMatrix() const;
	[[nodiscard]] Matrix4D GetPerspectiveMatrix() const;

	float m_fov = 60;
	float m_farZ = 100;
	float m_nearZ = 0.01f;
	float m_order = 0;

private:
	
	void UpdateConstantBuffer();

	Renderer::ConstantBuffer m_constantBuffer;

DEFINE_CLASS()
};

CREATE_CLASS(CameraComponent)
