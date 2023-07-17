#include "stdafx.h"
#include "StaticMesh.h"

namespace Rendering
{
	StaticMesh::StaticMesh()
	{
		m_staticMeshComponent = AddComponent<StaticMeshComponent>();
	}
}
