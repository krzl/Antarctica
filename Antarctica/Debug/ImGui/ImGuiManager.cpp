#include "stdafx.h"
#include "Debug/ImGui/ImGuiManager.h"

#include "Archetypes/ArchetypeBuilder.h"
#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"
#include "Debug/ImGui/ImGuiComponent.h"

void ImGuiManager::DefineArchetype(ArchetypeBuilder& builder)
{
	Entity::DefineArchetype(builder);

	builder.AddComponent<Rendering::MeshComponent>();
	builder.AddComponent<TransformComponent>();
	builder.AddComponent<Rendering::RenderComponent>();
	builder.AddComponent<ImGuiComponent>();
}

void ImGuiManager::SetupComponents(ComponentAccessor& accessor)
{
	Entity::SetupComponents(accessor);
}
