#pragma once
#include "assimp/matrix4x4.h"

struct Skeleton;
struct AnimationNode;

struct BoneWeight
{
	uint32_t m_boneId = 0;
	float    m_weight = 0.0f;
};

struct VertexWeights
{
	BoneWeight m_boneWeights[4];
};

struct Bone
{
	const Bone* m_parent = nullptr;
	std::string m_boneName;
	Skeleton*   m_skeleton = nullptr;
	Transform4D m_offsetMatrix;
};

struct Skeleton
{
	std::vector<Bone>          m_bones;
	std::vector<VertexWeights> m_vertexWeights;
	Transform4D                m_globalInverseTransform;

	int32_t GetBoneId(const AnimationNode* node) const;

private:

	mutable std::map<const AnimationNode*, int32_t> m_lookupMap;
};
