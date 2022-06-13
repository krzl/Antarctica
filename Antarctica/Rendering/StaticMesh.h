#pragma once
#include "StaticMeshComponent.h"
#include "GameObjects/GameObject.h"

class StaticMesh : public GameObject
{
DEFINE_CLASS()

public:

	StaticMesh();

	Ref<StaticMeshComponent> GetStaticMeshComponent() const
	{
		return m_staticMeshComponent;
	}

protected:

	Ref<StaticMeshComponent> m_staticMeshComponent;
};

CREATE_CLASS(StaticMesh)
