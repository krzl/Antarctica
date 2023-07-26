#pragma once

#include "DebugDrawComponent.h"
#include "Debug/DebugDrawManager.h"
#include "Systems/System.h"

namespace Rendering
{
	struct MeshComponent;
}

class DebugDrawSystem : public System<DebugDrawComponent, Rendering::MeshComponent>
{
	void Init() override;
	void Update(uint64_t entityId, DebugDrawComponent* debugDraw, Rendering::MeshComponent* mesh) override;
	void OnUpdateEnd() override;

	std::vector<std::shared_ptr<DebugDrawManager::ElementBuilder>> m_awaitingElements;
	std::vector<float>                                             m_despawnTimes;

	std::shared_ptr<Shader> m_shader;
	AttributeUsage          m_attributeUsage = {};
};
