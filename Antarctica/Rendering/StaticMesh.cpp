#include "stdafx.h"
#include "StaticMesh.h"

StaticMesh::StaticMesh()
{
	m_staticMeshComponent = AddComponent<StaticMeshComponent>();
}
