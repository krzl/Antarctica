#include "stdafx.h"
#include "MaterialObject.h"

#include "RenderSystem.h"

void Renderer::MaterialObject::UpdateAndBind() const
{
	const ShaderDescriptor& descriptor = GetShaderObject().GetShaderDescriptor();

	for (auto& elem : descriptor.GetTextures())
	{
		auto it = m_textures.find(elem.m_name);
		if (it != m_textures.end())
		{
			it->second->GetTextureObject().Bind(elem.m_id);
		}
	}
}
