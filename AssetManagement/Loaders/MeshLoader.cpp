#include "stdafx.h"
#include "AssetLoader.h"

#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <Assets/SubmeshData.h>

#include "AnimationLoader.h"
#include "SubmeshBuilder.h"

std::vector<Vector3D> CastToVector(const uint32_t count, const aiVector3D& input)
{
	const auto inputArray = reinterpret_cast<const Vector3D*>(&input);
	return std::vector<Vector3D>(inputArray, inputArray + count);
}

template<>
std::shared_ptr<Mesh> AssetLoader::Load(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(),
										aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PopulateArmatureData);

	std::vector<MeshBuffer> indexDataList;
	std::vector<MeshBuffer> vertexDataList;

	auto mesh = std::make_shared<Mesh>();

	if (!scene)
	{
		return mesh;
	}

	Transform4D globalTransformMatrix;
	memcpy(globalTransformMatrix.matrix.data, &scene->mRootNode->mTransformation.a1, sizeof(globalTransformMatrix));
	mesh->SetGlobalInverseMatrix(Inverse(globalTransformMatrix));

	for (uint32_t meshId = 0; meshId < scene->mNumMeshes; ++meshId)
	{
		aiMesh* submesh = scene->mMeshes[meshId];

		std::vector<uint32_t> indices(submesh->mNumFaces * 3);

		for (uint32_t i = 0; i < submesh->mNumFaces; i++)
		{
			memcpy(&indices[i * 3], submesh->mFaces[i].mIndices, sizeof uint32_t * 3);
		}

		std::vector<Vector3D> vertices = CastToVector(submesh->mNumVertices, *submesh->mVertices);

		SubmeshBuilder builder(std::move(vertices), indices);

		if (submesh->HasNormals())
		{
			builder.SetNormals(CastToVector(submesh->mNumVertices, *submesh->mNormals));
		}
		if (submesh->HasTangentsAndBitangents())
		{
			builder.SetTangents(CastToVector(submesh->mNumVertices, *submesh->mTangents));
			builder.SetNormals(CastToVector(submesh->mNumVertices, *submesh->mBitangents));
		}
		if (submesh->GetNumColorChannels() > 0)
		{
			std::vector<Vector4D> colors(submesh->GetNumColorChannels() * submesh->mNumVertices);
			for (uint32_t i = 0; i < submesh->GetNumColorChannels(); i++)
			{
				memcpy(&colors[i * submesh->mNumVertices], submesh->mColors[0],
					   sizeof submesh->mColors[0] * submesh->mNumVertices);
			}
			builder.SetColors(std::move(colors));
		}
		for (uint32_t uvChannel = 0; uvChannel < Min(submesh->GetNumUVChannels(), 4u); uvChannel++)
		{
			const uint8_t      channelCount = submesh->mNumUVComponents[uvChannel];
			std::vector<float> texcoords(channelCount * submesh->mNumVertices);
			for (uint32_t i = 0; i < submesh->mNumVertices; i++)
			{
				memcpy(&texcoords[i * channelCount], &submesh->mTextureCoords[uvChannel][i],
					   sizeof(float) * channelCount);
			}
			builder.SetTexcoords(std::move(texcoords), uvChannel);
		}
		if (submesh->HasBones())
		{
			Skeleton skeleton;
			skeleton.m_bones.resize(submesh->mNumBones);
			skeleton.m_vertexWeights.resize(submesh->mNumVertices);
			skeleton.m_globalInverseTransform = Inverse(AIMatrixCast(scene->mRootNode->mTransformation));

			std::unordered_map<aiNode*, Bone*> nodeToBoneMap(submesh->mNumBones);

			std::vector<uint8_t> weightCounters(submesh->mNumVertices);
			
			for (uint32_t boneId = 0; boneId < submesh->mNumBones; ++boneId)
			{
				Bone&         bone   = skeleton.m_bones[boneId];
				const aiBone* aiBone = submesh->mBones[boneId];

				Bone*   parentBone        = nullptr;
				aiNode* currentSearchNode = aiBone->mNode->mParent;
				while (parentBone == nullptr && currentSearchNode != nullptr)
				{
					auto it           = nodeToBoneMap.find(currentSearchNode);
					parentBone        = it != nodeToBoneMap.end() ? it->second : nullptr;
					currentSearchNode = currentSearchNode->mParent;
				}

				bone.m_skeleton = &skeleton;
				bone.m_parent   = parentBone;
				bone.m_boneName = aiBone->mName.C_Str();
				bone.m_offsetMatrix = AIMatrixCast(aiBone->mOffsetMatrix);

				for (uint32_t weightId = 0; weightId < aiBone->mNumWeights; ++weightId)
				{
					const uint32_t vertexId                                    = aiBone->mWeights[weightId].mVertexId;
					skeleton.m_vertexWeights[vertexId].m_boneWeights[weightCounters[vertexId]++] = BoneWeight{
						boneId,
						aiBone->mWeights[weightId].mWeight,
					};
				}

				nodeToBoneMap[aiBone->mNode] = &bone;
			}

			builder.SetSkeleton(std::move(skeleton));
		}

		mesh->AddSubmesh(builder.Build());
	}

	for (uint32_t animationId = 0; animationId < scene->mNumAnimations; ++animationId)
	{
		aiAnimation* animation = scene->mAnimations[animationId];
		mesh->AddAnimation(ImportAnimation(animation, scene->mRootNode));
	}

	aiReleaseImport(scene);

	return mesh;
}
