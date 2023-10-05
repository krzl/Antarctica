#pragma once

#include "Components/Component.h"
#include "PhysicsBody/CollisionData.h"

namespace Physics
{
	struct PhysicsBodyComponent : Component
	{
		float GetInverseMass() const { return m_inverseMass; }
		float GetMass() const { return m_mass; }
		void SetMass(float mass);

		std::vector<CollisionData> m_collisions;

		float m_restitution     = 0.05f;
		float m_staticFriction  = 0.1f;
		float m_dynamicFriction = 0.2f;

	private:

		float m_mass        = 0.0f;
		float m_inverseMass = 0.0f;

	public:

		DEFINE_CLASS()
	};

	CREATE_CLASS(PhysicsBodyComponent)

	inline void PhysicsBodyComponent::SetMass(const float mass)
	{
		m_mass        = mass;
		m_inverseMass = 1.0f / mass;
	}
}
