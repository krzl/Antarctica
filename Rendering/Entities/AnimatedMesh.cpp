#include "stdafx.h"
#include "AnimatedMesh.h"

namespace Rendering
{
	AnimatedMesh::AnimatedMesh()
	{
		m_animatedMeshComponent = AddComponent<AnimatedMeshComponent>();
	}
}
