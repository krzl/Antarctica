#pragma once

namespace Physics
{
	struct CollisionData
	{
		Entity* m_entityA = nullptr;
		Entity* m_entityB = nullptr;

		float m_restitution = 0.0f;

		float m_staticFriction  = 0.0f;
		float m_dynamicFriction = 0.0f;

		float m_penetration = 0.0f;
		Vector2D m_normal = Vector2D::zero;
		Point2D m_contactPoint = Point2D();
	};
}
