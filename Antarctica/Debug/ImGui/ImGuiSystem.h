#pragma once

#include <memory>

#include "imgui.h"
#include "Assets/DynamicSubmesh.h"
#include "Assets/Material.h"
#include "Buffers/Types/PerObjectBuffer.h"
#include "Systems/System.h"

class ImGuiShader;

namespace Renderer
{
	struct QueuedRenderObject;
}

struct Submesh;

class ImGuiSystem : public System
{
	REGISTER_SYSTEM(ImGuiSystem);

protected:

	void  Init() override;
	void  Update() override;

public:

	std::vector<Renderer::QueuedRenderObject>& Render();

private:

	Renderer::PerObjectBuffer perObjectBuffer = {};

	std::shared_ptr<Material>    m_material;
	std::shared_ptr<ImGuiShader> m_shader;
	std::shared_ptr<Texture>     m_texture;

	std::vector<DynamicSubmesh> m_submeshes;

	std::vector<Renderer::QueuedRenderObject> m_renderObjectsCache;

	AttributeUsage m_attributeUsage;
};
