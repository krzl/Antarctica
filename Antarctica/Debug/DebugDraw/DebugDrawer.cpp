#include "stdafx.h"
#include "Debug/DebugDraw/DebugDrawer.h"

#include "DebugDrawComponent.h"
#include "Archetypes/ArchetypeBuilder.h"
#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"

void DebugDrawer::DefineArchetype(ArchetypeBuilder& builder)
{
	Entity::DefineArchetype(builder);

	builder.AddComponent<Rendering::MeshComponent>();
	builder.AddComponent<TransformComponent>();
	builder.AddComponent<Rendering::RenderComponent>();
	builder.AddComponent<DebugDrawComponent>();
	builder.AddComponent<Rendering::RenderCullComponent>();
}

void DebugDrawer::SetupComponents(ComponentAccessor& accessor)
{
	Entity::SetupComponents(accessor);
}
