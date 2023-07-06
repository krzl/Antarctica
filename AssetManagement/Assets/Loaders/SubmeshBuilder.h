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

	[[nodiscard]] const std::string&           GetName() const { return m_name; }
	[[nodiscard]] const std::vector<uint8_t>&  GetIndices() const { return m_indices; }
	[[nodiscard]] const std::vector<Vector3D>& GetPositions() const { return m_positions; }
	[[nodiscard]] const std::vector<Vector3D>& GetNormals() const { return m_normals; }
	[[nodiscard]] const std::vector<Vector3D>& GetTangents() const { return m_tangents; }
	[[nodiscard]] const std::vector<Vector3D>& GetBitangents() const { return m_bitangents; }
	[[nodiscard]] const std::vector<Vector4D>& GetColors() const { return m_colors; }
	[[nodiscard]] const std::vector<float>&    GetTexcoords0() const { return m_texcoords0; }
	[[nodiscard]] const std::vector<float>&    GetTexcoords1() const { return m_texcoords1; }
	[[nodiscard]] const std::vector<float>&    GetTexcoords2() const { return m_texcoords2; }
	[[nodiscard]] const std::vector<float>&    GetTexcoords3() const { return m_texcoords3; }
	[[nodiscard]] const Skeleton&              GetSkeleton() const { return m_skeleton; }

	Submesh Build();

private:

	AttributeUsage          GetAttributeUsage() const;

	BoundingBox CalculateBoundingBox() const;

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
