#pragma once
#include "StaticMeshComponent.h"
#include "GameObjects/GameObject.h"

namespace Renderer
{
	class StaticMesh : public GameObject
	{
	public:

		StaticMesh();

		Ref<StaticMeshComponent> GetStaticMeshComponent() const
		{
			return m_staticMeshComponent->GetRef().Cast<StaticMeshComponent>();
		}

	protected:

		Ref<StaticMeshComponent> m_staticMeshComponent;

		DEFINE_CLASS()
	};

	CREATE_CLASS(StaticMesh)
}
