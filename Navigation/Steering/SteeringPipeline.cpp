#include "stdafx.h"
#include "SteeringPipeline.h"

#include "Behaviors/SteeringBehavior.h"
#include "Components/MovementComponent.h"

namespace Navigation
{
	void SteeringPipeline::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) const
	{
		InitializeTotalAccelerationCalculation(movement->m_arriveBehavior, transform, movement);
		InitializeTotalAccelerationCalculation(movement->m_alignmentBehavior, transform, movement);
		InitializeTotalAccelerationCalculation(movement->m_avoidanceBehavior, transform, movement);
		InitializeTotalAccelerationCalculation(movement->m_cohesionBehavior, transform, movement);
		InitializeTotalAccelerationCalculation(movement->m_separationBehavior, transform, movement);
	}

	void SteeringPipeline::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											  const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement) const
	{
		UpdateNearbyEntity(movement->m_arriveBehavior, transform, movement, nearbyTransform, nearbyMovement);
		UpdateNearbyEntity(movement->m_alignmentBehavior, transform, movement, nearbyTransform, nearbyMovement);
		UpdateNearbyEntity(movement->m_avoidanceBehavior, transform, movement, nearbyTransform, nearbyMovement);
		UpdateNearbyEntity(movement->m_cohesionBehavior, transform, movement, nearbyTransform, nearbyMovement);
		UpdateNearbyEntity(movement->m_separationBehavior, transform, movement, nearbyTransform, nearbyMovement);
	}

	Vector2D SteeringPipeline::GetFinalLinearAcceleration(const TransformComponent* transform, MovementComponent* movement) const
	{
		Vector2D totalAcceleration = Vector2D::zero;

		totalAcceleration += GetLinearAcceleration(movement->m_arriveBehavior, transform, movement);
		totalAcceleration += GetLinearAcceleration(movement->m_alignmentBehavior, transform, movement);
		totalAcceleration += GetLinearAcceleration(movement->m_avoidanceBehavior, transform, movement);
		totalAcceleration += GetLinearAcceleration(movement->m_cohesionBehavior, transform, movement);
		totalAcceleration += GetLinearAcceleration(movement->m_separationBehavior, transform, movement);

		const float accelerationMagnitudeSqr = SquaredMag(totalAcceleration);
		//if (accelerationMagnitudeSqr > movement->m_maxAcceleration * movement->m_maxAcceleration)
		if (accelerationMagnitudeSqr != 0.0f)
		{
			totalAcceleration = totalAcceleration * (movement->m_maxAcceleration / Terathon::Sqrt(accelerationMagnitudeSqr));
		}

		return totalAcceleration;
	}

	void SteeringPipeline::InitializeTotalAccelerationCalculation(SteeringBehavior& steeringBehavior, const TransformComponent* transform,
																  MovementComponent* movement)
	{
		if (steeringBehavior.GetWeight() != 0.0f)
		{
			steeringBehavior.InitializeTotalAccelerationCalculation(transform, movement);
		}
	}

	void SteeringPipeline::UpdateNearbyEntity(SteeringBehavior& steeringBehavior, const TransformComponent* transform, MovementComponent* movement,
											  const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		if (steeringBehavior.GetWeight() != 0.0f)
		{
			steeringBehavior.UpdateNearbyEntity(transform, movement, nearbyTransform, nearbyMovement);
		}
	}

	Vector2D SteeringPipeline::GetLinearAcceleration(SteeringBehavior& steeringBehavior, const TransformComponent* transform,
													 const MovementComponent* movement) const
	{
		const float weight = steeringBehavior.GetWeight();

		if (weight == 0.0f)
		{
			return Vector2D::zero;
		}

		const Vector2D acceleration = steeringBehavior.GetFinalLinearAcceleration(transform, movement);

		if (IsNaN(acceleration))
		{
			return Vector2D::zero;
		}

		return acceleration * weight;
	}
}
