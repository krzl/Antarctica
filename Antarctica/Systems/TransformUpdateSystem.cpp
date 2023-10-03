#include "stdafx.h"
#include "TransformUpdateSystem.h"

#include "Archetypes/ComponentAccessor.h"

#include "Components/MovementComponent.h"

#include "Core/Application.h"

#include "Entities/World.h"

#include "Managers/FrameCounter.h"
#include "Managers/TimeManager.h"

void TransformUpdateSystem::OnUpdateStart()
{
	System::OnUpdateStart();
	m_stepLockProgress = Min(1.0f, m_frameCounter->GetNextStepLockProgress());
}

void TransformUpdateSystem::Update(const uint64_t entityId, TransformComponent* transform, MoveableComponent* moveable,
								   Rendering::RenderComponent* render)
{
	const ComponentAccessor& componentAccessor    = World::Get()->GetEntity(entityId)->GetComponentAccessor();
	const Navigation::MovementComponent* movement = componentAccessor.GetComponent<Navigation::MovementComponent>();

	if (movement != nullptr && movement->m_velocity != Vector2D::zero)
	{
		const float deltaTime = TimeManager::GetTimeStep() * m_stepLockProgress;

		Point3D newLocalPosition = transform->m_localPosition + Vector3D(movement->m_velocity, 0.0f) * deltaTime;

		newLocalPosition.z = Application::Get().GetGameState().GetTerrain()->GetHeightAtLocation((Point2D) newLocalPosition.xy);

		const Vector3D oldDirection = transform->m_localRotation.GetDirectionY();
		const float oldOrientation  = std::atan2(-oldDirection.x, oldDirection.y);

		const Vector2D velocityDir    = Normalize(movement->m_velocity);
		const float targetOrientation = std::atan2(-velocityDir.x, velocityDir.y);

		const float rotation     = RemapAngleRad(targetOrientation - oldOrientation);
		const float rotationSize = abs(rotation);

		Quaternion newLocalRotation = transform->m_localRotation;

		if (rotationSize != 0.0f)
		{
			const float diff = abs(Min(movement->m_maxRotation * deltaTime, rotationSize)) * rotation / rotationSize;

			const float newOrientation = oldOrientation + diff;

			if (abs(AngleDifference(oldOrientation, targetOrientation)) < abs(AngleDifference(oldOrientation, newOrientation)))
			{
				newLocalRotation = Quaternion::MakeRotationZ(targetOrientation);
			}
			else
			{
				newLocalRotation = Quaternion::MakeRotationZ(newOrientation);
			}
		}

		render->m_worldTransform = Transform4D::MakeTranslation(newLocalPosition) *
			newLocalRotation.GetRotationMatrix() *
			Transform4D::MakeScale(transform->m_localScale.x, transform->m_localScale.y, transform->m_localScale.z);
	}
	else
	{
		render->m_worldTransform = transform->GetWorldTransform();
	}
}
