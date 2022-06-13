#pragma once

#include <Assets/Material.h>
#include <Assets/Mesh.h>

#include "RenderComponent.h"
#include "Components/SceneComponent.h"

class StaticMeshComponent : public RenderComponent
{
	
	DEFINE_CLASS()

public:

	[[nodiscard]] const std::shared_ptr<Mesh>& GetMesh() const
	{
		return m_mesh;
	}

	void SetMesh(const std::shared_ptr<Mesh>& mesh)
	{
		m_mesh = mesh;
	}

	[[nodiscard]] std::shared_ptr<Material> GetMaterial()
	{
		return m_materials.size() > 0 ? m_materials[0] : std::shared_ptr<Material>(nullptr);
	}

	[[nodiscard]] const std::vector<std::shared_ptr<Material>>& GetMaterials() const
	{
		return m_materials;
	}

	void SetMaterial(const std::shared_ptr<Material> material, const uint32_t index = 0)
	{
		assert(index < 8);
		if (m_materials.size() <= index)
		{
			while (m_materials.size() <= index)
			{
				m_materials.push_back(m_materials.size() == index ? material : nullptr);
			}
		}
		else
		{
			m_materials[index] = material;
		}
	}

	void SetMaterials(const std::vector<std::shared_ptr<Material>>& materials)
	{
		m_materials = materials;
	}

protected:

	virtual std::vector<Renderer::RenderHandle> PrepareForRender() override;

private:

	std::shared_ptr<Mesh> m_mesh;
	std::vector<std::shared_ptr<Material>> m_materials;
};

CREATE_CLASS(StaticMeshComponent)