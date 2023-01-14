#pragma once

#include "../Asset.h"

#include <Objects/MeshObject.h>

namespace Renderer
{
	struct Submesh;
}

struct BoneWeight
{
	uint32_t m_vertexId = 0;
	float    m_weight   = 0.0f;
};

struct Bone
{
	Bone() :
		m_parent(nullptr) { }

	Bone(const Bone* parent, std::vector<BoneWeight>&& weights)
		: m_parent(parent),
		  m_weights(std::move(weights)) { }

	const Bone*             m_parent = nullptr;
	std::vector<BoneWeight> m_weights;
};

struct Skeleton
{
	std::vector<Bone> m_bones;
};

class Mesh : public Asset
{
public:

	void AddSubmesh(Renderer::Submesh&& submesh);

	[[nodiscard]] uint32_t GetSubmeshCount() const
	{
		return (uint32_t) m_submeshes.size();
	}

	[[nodiscard]] const Renderer::Submesh& GetSubmesh(const uint32_t i) const
	{
		assert(i < GetSubmeshCount());
		return m_submeshes[i];
	}

	[[nodiscard]] const std::vector<Renderer::Submesh>& GetSubmeshes() const
	{
		return m_submeshes;
	}

	[[nodiscard]] Renderer::MeshObject& GetMeshObject()
	{
		return m_meshObject;
	}

	[[nodiscard]] const Renderer::MeshObject& GetMeshObject() const
	{
		return m_meshObject;
	}

	[[nodiscard]] const Transform4D& GetGlobalInverseMatrix() const
	{
		return m_globalInverseMatrix;
	}

	void SetGlobalInverseMatrix(const Transform4D& globalInverseMatrix)
	{
		m_globalInverseMatrix = globalInverseMatrix;
	}

private:

	Renderer::MeshObject           m_meshObject;
	std::vector<Renderer::Submesh> m_submeshes;
	Transform4D                    m_globalInverseMatrix = Transform4D::identity;
};
