#include "stdafx.h"
#include "Assets/Mesh.h"

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
	return std::vector(inputArray, inputArray + count);
}

Capsule GetCapsuleFromVertices(const std::vector<Point3D>& points)
{
	float biggestDistance = 0;

	Point3D a;
	Point3D b;

	for (uint32_t i = 0; i < points.size(); ++i)
	{
		for (uint32_t j = 0; j < points.size(); ++j)
		{
			if (i == j)
			{
				continue;
			}

			const Vector3D distance = a - b;

			if (biggestDistance < Dot(distance, distance))
			{
				a = points[i];
				b = points[j];
			}
		}
	}

	Capsule capsule;
	capsule.m_up     = (a - b).Normalize();
	capsule.m_center = (a - b) / 2 + b;
	capsule.m_height = std::sqrt(biggestDistance) / 2.0f;

	float widthSquared = 0.0f;
	for (uint32_t i = 0; i < points.size(); i++)
	{
		widthSquared = GetDistanceFromLineToPoint(capsule.m_up, points[i]);
	}

	capsule.m_width = std::sqrt(widthSquared);

	return capsule;
}

static void ProcessMeshNodeData(Mesh& mesh, const aiNode* node, const int32_t parentNodeId, std::vector<MeshNode>& nodes, Transform4D globalTransform)
{
	const uint32_t index = static_cast<uint32_t>(nodes.size());

	const Transform4D localTransform = AIMatrixCast(node->mTransformation);
	globalTransform                  = globalTransform * localTransform;

	std::string name        = node->mName.C_Str();
	const uint64_t nameHash = std::hash<std::string>()(name);

	const MeshNode& meshNode = nodes.emplace_back(MeshNode
		{
			std::move(name),
			nameHash,
			parentNodeId,
			globalTransform,
			localTransform
		}
	);

	for (const Submesh& submesh : mesh.GetSubmeshes())
	{
		if (submesh.GetName() == meshNode.m_name)
		{
			bool ignoreRotation = false;
			if (node->mMetaData)
			{
				node->mMetaData->Get("RotationActive", ignoreRotation);
			}
			submesh.SetupNodeAttachment(index, ignoreRotation);
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessMeshNodeData(mesh, node->mChildren[i], index, nodes, globalTransform);
	}
}

bool Mesh::Load(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(),
		aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PopulateArmatureData | aiProcess_FlipWindingOrder);

	std::vector<MeshBuffer> indexDataList;
	std::vector<MeshBuffer> vertexDataList;

	if (!scene)
	{
		return false;
	}

	Transform4D globalTransformMatrix = AIMatrixCast(scene->mRootNode->mTransformation);
	SetGlobalInverseMatrix(Inverse(globalTransformMatrix));

	for (uint32_t meshId = 0; meshId < scene->mNumMeshes; ++meshId)
	{
		aiMesh* submesh = scene->mMeshes[meshId];

		std::vector<uint32_t> indices(submesh->mNumFaces * 3);

		for (uint32_t i = 0; i < submesh->mNumFaces; i++)
		{
			memcpy(&indices[i * 3], submesh->mFaces[i].mIndices, sizeof uint32_t * 3);
		}

		std::vector<Vector3D> vertices = CastToVector(submesh->mNumVertices, *submesh->mVertices);

		SubmeshBuilder builder(submesh->mName.C_Str(), std::move(vertices), indices);

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
			const uint8_t channelCount = submesh->mNumUVComponents[uvChannel];
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
			skeleton.m_globalInverseTransform = AIMatrixCast(scene->mRootNode->mTransformation);

			std::unordered_map<aiNode*, Bone*> nodeToBoneMap(submesh->mNumBones);

			std::vector<uint8_t> weightCounters(submesh->mNumVertices);

			for (uint32_t boneId = 0; boneId < submesh->mNumBones; ++boneId)
			{
				Bone& bone           = skeleton.m_bones[boneId];
				const aiBone* aiBone = submesh->mBones[boneId];

				bone.m_skeleton     = &skeleton;
				bone.m_boneName     = aiBone->mName.C_Str();
				bone.m_boneNameHash = std::hash<std::string>()(bone.m_boneName);
				bone.m_offsetMatrix = AIMatrixCast(aiBone->mOffsetMatrix);

				uint32_t verticesFound = 0;
				std::vector<Point3D> verticesForCollider(aiBone->mNumWeights);

				for (uint32_t weightId = 0; weightId < aiBone->mNumWeights; ++weightId)
				{
					const uint32_t vertexId = aiBone->mWeights[weightId].mVertexId;

					skeleton.m_vertexWeights[vertexId].m_boneWeights[weightCounters[vertexId]++] = BoneWeight{
						boneId,
						aiBone->mWeights[weightId].mWeight,
					};

					if (aiBone->mWeights[weightId].mWeight > 0.5f)
					{
						verticesForCollider.emplace_back(builder.GetPositions()[aiBone->mWeights[weightId].mVertexId]);
						++verticesFound;
					}
				}

				verticesForCollider.resize(aiBone->mNumWeights);

				bone.m_collider = GetCapsuleFromVertices(verticesForCollider);

				nodeToBoneMap[aiBone->mNode] = &bone;
			}

			builder.SetSkeleton(std::move(skeleton));
		}

		AddSubmesh(builder.Build());
	}

	for (uint32_t animationId = 0; animationId < scene->mNumAnimations; ++animationId)
	{
		aiAnimation* aiAnimation             = scene->mAnimations[animationId];
		std::shared_ptr<Animation> animation = std::make_shared<Animation>();
		ImportAnimation(*animation, aiAnimation, scene->mRootNode);
		AddAnimation(animation);
	}

	std::vector<MeshNode> meshNodes;
	ProcessMeshNodeData(*this, scene->mRootNode, -1, meshNodes, Transform4D::identity);
	SetMeshNodeData(meshNodes);

	aiReleaseImport(scene);
	return true;
}
