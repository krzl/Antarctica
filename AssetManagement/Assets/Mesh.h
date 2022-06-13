#pragma once

#include <Objects/MeshObject.h>

#include "Asset.h"

namespace Renderer
{
	struct Submesh;
}

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
		return meshObject;
	}

	[[nodiscard]] const Renderer::MeshObject& GetMeshObject() const
	{
		return meshObject;
	}

private:

	Renderer::MeshObject meshObject;
	std::vector<Renderer::Submesh> m_submeshes;
};
