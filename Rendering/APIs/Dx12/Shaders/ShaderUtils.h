#pragma once

namespace Rendering
{
	struct ShaderDescriptor;
}

namespace Rendering::Dx12
{
	class ShaderStage;
	void UpdateDescriptor(const ShaderStage* shader, ShaderDescriptor& descriptor);
}
