#pragma once

#include "Entities/Entity.h"

class Texture;
class Material;
class ImGuiShader;

struct Submesh;

class ImGuiManager : public Entity
{
	void DefineArchetype(ArchetypeBuilder& builder) override;
	void SetupComponents(const ComponentAccessor& accessor) override;
};
