#pragma once

#include "ImGuiComponent.h"
#include "Assets/Material.h"
#include "Assets/SubmeshData.h"
#include "Systems/System.h"

class Texture;

namespace Rendering
{
	struct MeshComponent;
}

class ImGuiSystem : public System<ImGuiComponent, Rendering::MeshComponent>
{
	void Init() override;
	void OnFrameStart() override;
	void OnUpdateStart() override;
	void Update(Entity* entity, ImGuiComponent* imgui, Rendering::MeshComponent* mesh) override;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Material> m_material;
	std::shared_ptr<Texture> m_texture;

	AttributeUsage m_attributeUsage = {};

public:

	Dispatcher<> m_onNewFrame;
};
