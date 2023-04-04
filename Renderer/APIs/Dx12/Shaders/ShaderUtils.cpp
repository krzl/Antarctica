#include "stdafx.h"
#include "ShaderUtils.h"

#include "ComputeShader.h"
#include "ShaderStage.h"

namespace Renderer::Dx12
{
	void UpdateDescriptor(const ShaderStage* shader, ShaderDescriptor& descriptor)
	{
		ID3D12ShaderReflection&  reflector  = *shader->GetReflector();
		const D3D12_SHADER_DESC& shaderDesc = shader->GetDescriptor();

		for (uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflector.GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC              bufferDesc;
			constantBuffer->GetDesc(&bufferDesc);

			std::string bufferName = bufferDesc.Name;

			if (descriptor.ContainsBuffer(bufferName))
			{
				continue;
			}

			const auto defaultBufferValue = new uint8_t[bufferDesc.Size];

			for (uint32_t j = 0; j < bufferDesc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC      variableDesc;
				variable->GetDesc(&variableDesc);

				descriptor.AddVariableDescriptor(ShaderDescriptor::VariableDescriptor{
					std::string(variableDesc.Name),
					(uint16_t) i,
					(uint16_t) variableDesc.StartOffset,
					(uint16_t) variableDesc.Size
				});


				if (variableDesc.DefaultValue != nullptr)
				{
					memcpy(defaultBufferValue + variableDesc.StartOffset, variableDesc.DefaultValue, variableDesc.Size);
				}
				else
				{
					memset(defaultBufferValue + variableDesc.StartOffset, 0, variableDesc.Size);
				}
			}

			descriptor.AddBufferDescriptor(ShaderDescriptor::BufferDescriptor{
				std::move(bufferName),
				(uint16_t) bufferDesc.Size,
				defaultBufferValue
			});
		}

		for (uint32_t i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC desc;
			reflector.GetResourceBindingDesc(i, &desc);

			if (desc.Type == D3D_SIT_TEXTURE)
			{
				if (!descriptor.ContainsTextureId(desc.BindPoint))
				{
					descriptor.AddTextureDescriptor(ShaderDescriptor::TextureDescriptor{
						std::string(desc.Name),
						(uint16_t) desc.BindPoint
					});
				}
			}
		}
	}
}
