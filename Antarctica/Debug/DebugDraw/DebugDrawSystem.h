#pragma once

#include "DebugDrawComponent.h"
#include "Assets/Material.h"
#include "Assets/SubmeshData.h"
#include "Components/MeshComponent.h"
#include "Debug/DebugDrawManager.h"
#include "Systems/System.h"

class DebugDrawSystem : public System<DebugDrawComponent, Rendering::MeshComponent>
{
	void Init(FrameCounter* frameCounter) override;
	void Update(uint64_t entityId, DebugDrawComponent* debugDraw, Rendering::MeshComponent* mesh) override;
	void OnUpdateEnd() override;
	bool IsLockStepSystem() override { return false; }

	std::vector<std::shared_ptr<DebugDrawManager::ElementBuilder>> m_awaitingElements;
	std::vector<float>                                             m_despawnTimes;

	std::shared_ptr<Shader> m_shader;
	AttributeUsage          m_attributeUsage = {};
};
