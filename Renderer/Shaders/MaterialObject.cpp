#include "stdafx.h"
#include "MaterialObject.h"

#include "RenderSystem.h"

void Renderer::MaterialObject::UpdateAndBind() const
{
	const ShaderDescriptor& descriptor = GetShaderObject().GetShaderDescriptor();

	for (const auto& [name, id] : descriptor.GetTextures())
	{
		auto it = m_textures.find(name);
		if (it != m_textures.end())
		{
			it->second->GetTextureObject().Bind(id);
		}
	}
}
