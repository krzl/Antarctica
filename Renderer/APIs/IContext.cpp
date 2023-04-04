#include "stdafx.h"
#include "IContext.h"

#include "Assets/Shader.h"
#include "Dx12/Context.h"

namespace Renderer
{
	IContext* IContext::CreateContext()
	{
		return new Dx12::Dx12Context();
	}
}
