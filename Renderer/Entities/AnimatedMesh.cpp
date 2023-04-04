#include "stdafx.h"
#include "AnimatedMesh.h"

namespace Renderer
{
	AnimatedMesh::AnimatedMesh()
	{
		m_animatedMeshComponent = AddComponent<AnimatedMeshComponent>();
	}
}
