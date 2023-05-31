#pragma once

#include <Assets/SubmeshData.h>

class SubmeshBuilder
{
public:

	explicit SubmeshBuilder(std::string&& name, std::vector<Vector3D>&& positions, std::vector<uint32_t>& indices);

	void SetNormals(std::vector<Vector3D>&& normals);
	void SetTangents(std::vector<Vector3D>&& tangents);
	void SetBitangents(std::vector<Vector3D>&& bitangents);
	void SetColors(std::vector<Vector4D>&& colors);
	void SetTexcoords(std::vector<float>&& texcoords, uint8_t channel);
	void SetSkeleton(Skeleton&& skeleton);

	Submesh Build();

private:

	const AttributeOffsets& GetAttributeOffsets(const AttributeUsage& attributeUsage) const;
	AttributeUsage          GetAttributeUsage() const;

	std::string           m_name;
	std::vector<uint8_t>  m_indices;
	std::vector<Vector3D> m_positions;
	std::vector<Vector3D> m_normals;
	std::vector<Vector3D> m_tangents;
	std::vector<Vector3D> m_bitangents;
	std::vector<Vector4D> m_colors;
	std::vector<float>    m_texcoords0;
	std::vector<float>    m_texcoords1;
	std::vector<float>    m_texcoords2;
	std::vector<float>    m_texcoords3;
	Skeleton              m_skeleton;
};
