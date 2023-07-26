#pragma once

#include "ImGuiComponent.h"
#include "Assets/Material.h"
#include "Assets/SubmeshData.h"
#include "Systems/System.h"

class Texture;
class ImGuiShader;

namespace Rendering
{
	struct MeshComponent;
}

class ImGuiSystem : public System<ImGuiComponent, Rendering::MeshComponent>
{
	void Init() override;
	void OnFrameBegin() override;
	void OnUpdateStart() override;
	void Update(uint64_t entityId, ImGuiComponent* imgui, Rendering::MeshComponent* mesh) override;

	std::shared_ptr<ImGuiShader> m_shader;
	std::shared_ptr<Material> m_material;
	std::shared_ptr<Texture> m_texture;

	AttributeUsage m_attributeUsage = {};
};
