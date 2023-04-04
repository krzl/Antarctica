#pragma once

namespace Renderer
{
	struct ShaderDescriptor;
}

namespace Renderer::Dx12
{
	class ShaderStage;
	void UpdateDescriptor(const ShaderStage* shader, ShaderDescriptor& descriptor);
}
