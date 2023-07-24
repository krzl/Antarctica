#pragma once

#include "ImGuiComponent.h"
#include "Assets/SubmeshData.h"
#include "Components/MeshComponent.h"
#include "Systems/System.h"


class ImGuiSystem : public System<ImGuiComponent, Rendering::MeshComponent>
{
	void Init(FrameCounter* frameCounter) override;
	void OnFrameBegin() override;
	void OnUpdateStart() override;
	void Update(uint64_t entityId, ImGuiComponent* imgui, Rendering::MeshComponent* mesh) override;
	bool IsLockStepSystem() override { return false; }

	std::shared_ptr<ImGuiShader> m_shader;
	std::shared_ptr<Material>    m_material;
	std::shared_ptr<Texture>     m_texture;

	AttributeUsage m_attributeUsage = {};
};
