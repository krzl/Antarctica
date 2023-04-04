#pragma once

#include <CameraData.h>
#include <Buffers/ConstantBuffer.h>

#include "Components/SceneComponent.h"

namespace Renderer
{
	class CameraComponent : public SceneComponent
	{
		friend class Application;

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

		static std::priority_queue<CameraData> GetAllCameraData();

	protected:

		void OnEnabled() override;
		void OnDisabled() override;

		[[nodiscard]] Matrix4D GetLookAtMatrix() const;
		[[nodiscard]] Matrix4D GetPerspectiveMatrix() const;

		float m_fov   = 60;
		float m_farZ  = 100;
		float m_nearZ = 0.1f;
		float m_order = 0;

	private:

		void UpdateConstantBuffer();

		static void SetAspectRatio(const float aspectRatio);

		ConstantBuffer m_constantBuffer;

		inline static float m_aspectRatio = 1.0f;

		DEFINE_CLASS()
	};

	CREATE_CLASS(CameraComponent)
}
