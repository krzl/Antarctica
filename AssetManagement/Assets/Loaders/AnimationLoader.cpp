#include "stdafx.h"
#include "AnimationLoader.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "AnimationNodeBuilder.h"
#include "Assets/SubmeshData.h"

Quaternion AIQuaternionCast(const aiQuaternion& quaternion)
{
	return Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

Vector3D AIVectorCast(const aiVector3D& vector)
{
	return Vector3D(vector.x, vector.y, vector.z);
}

Transform4D AIMatrixCast(const aiMatrix4x4& matrix)
{
	return Transform4D(Matrix3D(matrix.a1, matrix.a2, matrix.a3, matrix.b1, matrix.b2, matrix.b3, matrix.c1, matrix.c2, matrix.c3),
		Vector3D(matrix.a4, matrix.b4, matrix.c4));
}

const aiNodeAnim* FindNode(const aiAnimation* animation, const std::string nodeName)
{
	for (uint32_t i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* node = animation->mChannels[i];
		if (std::string(node->mNodeName.data) == nodeName)
		{
			return node;
		}
	}

	return nullptr;
}

uint32_t GetChildNodeCount(const aiNode* aiNode)
{
	uint32_t nodeCount = 0;
	for (uint32_t i = 0; i < aiNode->mNumChildren; ++i)
	{
		nodeCount += GetChildNodeCount(aiNode->mChildren[i]);
	}

	return nodeCount + aiNode->mNumChildren;
}

AnimationNode& ImportNode(std::vector<AnimationNode>& nodes, float& duration, const aiAnimation* aiAnimation, const aiNode* aiNode,
						  uint32_t& currentIndex)
{
	AnimationNodeBuilder builder(std::string(aiNode->mName.C_Str()), AIMatrixCast(aiNode->mTransformation));

	const double tickScale = aiAnimation->mTicksPerSecond != 0 ? 1.0f / aiAnimation->mTicksPerSecond : 1;

	bool useRotation = true;
	if (aiNode->mMetaData)
	{
		aiNode->mMetaData->Get("Show", useRotation);
	}

	const aiNodeAnim* nodeAnim = FindNode(aiAnimation, builder.GetNodeName());
	if (nodeAnim && useRotation)
	{
		std::vector<PositionKey> positionKeys;
		positionKeys.reserve(nodeAnim->mNumPositionKeys);
		for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys; ++i)
		{
			positionKeys.emplace_back(
				PositionKey{
					AIVectorCast(nodeAnim->mPositionKeys[i].mValue),
					(float) (nodeAnim->mPositionKeys[i].mTime * tickScale)
				});
		}

		std::vector<RotationKey> rotationKeys;
		rotationKeys.reserve(nodeAnim->mNumRotationKeys);
		for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys; ++i)
		{
			rotationKeys.emplace_back(
				RotationKey{
					AIQuaternionCast(nodeAnim->mRotationKeys[i].mValue),
					(float) (nodeAnim->mRotationKeys[i].mTime * tickScale)
				});
		};

		std::vector<ScaleKey> scaleKeys;
		scaleKeys.reserve(nodeAnim->mNumScalingKeys);
		for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys; ++i)
		{
			scaleKeys.emplace_back(
				ScaleKey{
					AIVectorCast(nodeAnim->mScalingKeys[i].mValue),
					(float) (nodeAnim->mScalingKeys[i].mTime * tickScale)
				});
		}

		if (positionKeys.size() > 0)
		{
			duration = max(duration, positionKeys.back().m_time);
		}
		if (rotationKeys.size() > 0)
		{
			duration = max(duration, rotationKeys.back().m_time);
		}
		if (scaleKeys.size() > 0)
		{
			duration = max(duration, scaleKeys.back().m_time);
		}

		builder.SetPositionKeys(std::move(positionKeys));
		builder.SetRotationKeys(std::move(rotationKeys));
		builder.SetScaleKeys(std::move(scaleKeys));
	}

	const uint32_t id = currentIndex++;

	for (uint32_t i = 0; i < aiNode->mNumChildren; ++i)
	{
		AnimationNode& child = ImportNode(nodes, duration, aiAnimation, aiNode->mChildren[i], currentIndex);
		builder.AddChildren(child);
	}

	nodes[id] = builder.Build();

	return nodes[id];
}

void ImportAnimation(Animation& animation, const aiAnimation* aiAnimation, const aiNode* rootNode)
{
	animation.m_nodes.resize(GetChildNodeCount(rootNode) + 1);

	uint32_t currentIndex = 0;

	animation.m_rootNode = &ImportNode(animation.m_nodes, animation.m_duration, aiAnimation, rootNode, currentIndex);
}

bool Animation::Load(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PopulateArmatureData);

	std::vector<MeshBuffer> indexDataList;
	std::vector<MeshBuffer> vertexDataList;

	if (!scene || !scene->HasAnimations())
	{
		return false;
	}

	ImportAnimation(*this, scene->mAnimations[0], scene->mRootNode);

	aiReleaseImport(scene);

	return true;
}
