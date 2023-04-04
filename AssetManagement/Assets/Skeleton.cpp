#include "stdafx.h"
#include "Skeleton.h"

#include "Animation.h"

int32_t Skeleton::GetBoneId(const AnimationNode* node) const
{
	const auto it = m_lookupMap.find(node);

	if (it != m_lookupMap.end())
	{
		return it->second;
	}

	for (uint32_t i = 0; i < m_bones.size(); ++i)
	{
		if (m_bones[i].m_boneName == node->m_nodeName)
		{
			m_lookupMap[node] = i;
			return i;
		}
	}

	return -1;
}
