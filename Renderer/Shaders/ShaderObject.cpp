#include "stdafx.h"
#include "ShaderObject.h"

#include "RenderSystem.h"

namespace Renderer
{
	void ShaderObject::Bind() const
	{
		if (!IsCompiled())
		{
			const_cast<ShaderObject*>(this)->Compile();
		}

		RenderSystem::Get().GetCommandList()->SetPipelineState(m_pipelineState.Get());
		RenderSystem::Get().GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());
	}
}
