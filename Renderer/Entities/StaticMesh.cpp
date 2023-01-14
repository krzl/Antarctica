#include "stdafx.h"
#include "StaticMesh.h"

namespace Renderer
{
	StaticMesh::StaticMesh()
	{
		m_staticMeshComponent = AddComponent<StaticMeshComponent>();
	}
}
