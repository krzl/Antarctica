#pragma once

#include "Assets/DynamicSubmesh.h"
#include "Managers/Manager.h"

class Texture;
class Material;
class ImGuiShader;

namespace Rendering
{
	struct QueuedRenderObject;
}

struct Submesh;

class ImGuiManager : public Manager
{
	REGISTER_MANAGER(ImGuiManager);

protected:

	void Init() override;
	void Update() override;

public:

	std::vector<Rendering::QueuedRenderObject>& Render();

private:

	std::shared_ptr<Material>    m_material;
	std::shared_ptr<ImGuiShader> m_shader;
	std::shared_ptr<Texture>     m_texture;

	std::vector<DynamicSubmesh> m_submeshes;

	std::vector<Rendering::QueuedRenderObject> m_renderObjectsCache;

	AttributeUsage m_attributeUsage;

	bool m_wasRendered = true;
};
