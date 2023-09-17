#pragma once

#include "Mesh.h"

class DynamicMesh : public Mesh
{
public:

	~DynamicMesh()
	{
		
	}
	
	[[nodiscard]] std::vector<Submesh>& GetSubmeshes() { return m_submeshes; }
	
	[[nodiscard]] Submesh& GetSubmesh(const uint32_t i = 0) { return m_submeshes[i]; }

	void SetSubmeshCount(const uint32_t newCount)
	{
		m_submeshes.resize(newCount);
	}
};
