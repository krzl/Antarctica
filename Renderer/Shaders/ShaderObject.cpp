#include "stdafx.h"
#include "ShaderObject.h"

#include "RenderSystem.h"

namespace Renderer
{
	ShaderObject::~ShaderObject()
	{
		RELEASE_DX(m_vsReflector);
		RELEASE_DX(m_psReflector);
		RELEASE_DX(m_rootSignature);
		RELEASE_DX(m_vsByteCode);
		RELEASE_DX(m_psByteCode);
		RELEASE_DX(m_dsByteCode);
		RELEASE_DX(m_hsByteCode);
		RELEASE_DX(m_gsByteCode);
	}

	void ShaderObject::Bind(const AttributeUsage& attributeUsage) const
	{
		if (!IsCompiled())
		{
			const_cast<ShaderObject*>(this)->Compile();
		}

		RenderSystem::Get().GetCommandList()->SetPipelineState(GetPipelineState(attributeUsage));
		RenderSystem::Get().GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());
	}
}
