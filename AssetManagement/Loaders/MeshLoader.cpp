#include "stdafx.h"
#include "AssetLoader.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Objects/SubmeshData.h>

#include "SubmeshBuilder.h"

std::vector<Vector3D> CastToVector(const uint32_t count, const aiVector3D& input)
{
	const Vector3D* inputArray = reinterpret_cast<const Vector3D*>(&input);
	return std::vector<Vector3D>(inputArray, inputArray + count);
}

template<>
std::shared_ptr<Mesh> AssetLoader::Load(const std::string& path)
{
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	std::vector<Renderer::MeshBuffer> indexDataList;
	std::vector<Renderer::MeshBuffer> vertexDataList;

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	if (!scene)
	{
		return mesh;
	}

	for (uint32_t meshId = 0; meshId < scene->mNumMeshes; ++meshId)
	{
		aiMesh* submesh = scene->mMeshes[meshId];

		std::vector<uint32_t> indices(submesh->mNumFaces * 3);

		for (uint32_t i = 0; i < submesh->mNumFaces; i++)
		{
			memcpy(&indices[i * 3], submesh->mFaces[i].mIndices, sizeof uint32_t * 3);
		}

		std::vector<Vector3D> vertices = CastToVector(submesh->mNumVertices, *submesh->mVertices);
		
		SubmeshBuilder builder(std::move(vertices), std::move(indices));

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
				memcpy(&colors[i * submesh->mNumVertices], submesh->mColors[0], sizeof submesh->mColors[0] * submesh->mNumVertices);
			}
			builder.SetColors(std::move(colors));
		}
		for (uint32_t uvChannel = 0; uvChannel < Min(submesh->GetNumUVChannels(), 4u); uvChannel++)
		{
			const uint8_t channelCount = submesh->mNumUVComponents[uvChannel];
			std::vector<float> texcoords(channelCount * submesh->mNumVertices);
			for (uint32_t i = 0; i < submesh->mNumVertices; i++)
			{
				memcpy(&texcoords[i * channelCount], &submesh->mTextureCoords[uvChannel][i], sizeof(float) * channelCount);
			}
		}

		mesh->AddSubmesh(builder.Build());
	}
	
	aiReleaseImport(scene);

	mesh->GetMeshObject().Init(mesh->GetSubmeshes());
	
	return mesh;
}
