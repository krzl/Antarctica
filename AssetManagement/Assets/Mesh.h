#pragma once

#include "Asset.h"

#include "Animation.h"
#include "SubmeshData.h"

class Mesh : public Asset
{
public:

	void AddSubmesh(Submesh&& submesh);

	void AddAnimation(const std::shared_ptr<Animation> animation)
	{
		m_animations.push_back(animation);
	}

	[[nodiscard]] uint32_t GetAnimationCount() const
	{
		return (uint32_t) m_animations.size();
	}

	[[nodiscard]] std::shared_ptr<Animation> GetAnimation(const uint32_t i) const
	{
		assert(i < GetAnimationCount());
		return m_animations[i];
	}

	[[nodiscard]] const std::vector<std::shared_ptr<Animation>>& GetAnimations() const
	{
		return m_animations;
	}

	[[nodiscard]] uint32_t GetSubmeshCount() const
	{
		return (uint32_t) m_submeshes.size();
	}

	[[nodiscard]] const Submesh& GetSubmesh(const uint32_t i) const
	{
		assert(i < GetSubmeshCount());
		return m_submeshes[i];
	}

	[[nodiscard]] const std::vector<Submesh>& GetSubmeshes() const
	{
		return m_submeshes;
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

	std::vector<Submesh>                    m_submeshes;
	std::vector<std::shared_ptr<Animation>> m_animations;
	Transform4D                             m_globalInverseMatrix = Transform4D::identity;
};
