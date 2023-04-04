#include "stdafx.h"
#include "AnimationLoader.h"
#include "AssetLoader.h"

#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "AnimationNodeBuilder.h"

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
	return Transform4D(Matrix3D(matrix.a1, matrix.a2, matrix.a3, matrix.b1, matrix.b2, matrix.b3, matrix.c1, matrix.c2, matrix.c3), Vector3D(matrix.a4, matrix.b4, matrix.c4));
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

AnimationNode& ImportNode(std::vector<AnimationNode>& nodes, float&     duration, const aiAnimation* aiAnimation,
						  const aiNode*               aiNode, uint32_t& currentIndex)
{
	AnimationNodeBuilder builder(std::string(aiNode->mName.C_Str()), Inverse(AIMatrixCast(aiNode->mTransformation)));
	
	const double tickScale = aiAnimation->mTicksPerSecond != 0 ? 1.0f / aiAnimation->mTicksPerSecond : 1;
	
	if (const aiNodeAnim* nodeAnim = FindNode(aiAnimation, builder.GetNodeName()))
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
		}

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

	for (uint32_t i = 0; i < aiNode->mNumChildren; ++i)
	{
		AnimationNode& child = ImportNode(nodes, duration, aiAnimation, aiNode->mChildren[i], currentIndex);
		builder.AddChildren(child);
	}

	nodes[currentIndex] = builder.Build();

	return nodes[currentIndex++];
}

std::shared_ptr<Animation> ImportAnimation(const aiAnimation* aiAnimation, const aiNode* rootNode)
{
	std::vector<AnimationNode> nodes(GetChildNodeCount(rootNode) + 1);

	float    duration;
	uint32_t currentIndex = 0;

	AnimationNode& root = ImportNode(nodes, duration, aiAnimation, rootNode, currentIndex);

	std::shared_ptr<Animation> animation = std::make_shared<Animation>(std::move(nodes), root, duration);

	return animation;
}

template<>
std::shared_ptr<Animation> AssetLoader::Load(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(),
										aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PopulateArmatureData);

	std::vector<MeshBuffer> indexDataList;
	std::vector<MeshBuffer> vertexDataList;

	if (!scene || !scene->HasAnimations())
	{
		return std::make_shared<Animation>();
	}

	std::shared_ptr<Animation> animation = ImportAnimation(scene->mAnimations[0], scene->mRootNode);

	aiReleaseImport(scene);

	return animation;
}
