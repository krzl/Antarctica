#include "stdafx.h"
#include "ShaderUtils.h"

#include "ComputeShader.h"
#include "ShaderStage.h"

namespace Rendering::Dx12
{
	void UpdateDescriptor(const ShaderStage* shader, ShaderDescriptor& descriptor)
	{
		ID3D12ShaderReflection& reflector   = *shader->GetReflector();
		const D3D12_SHADER_DESC& shaderDesc = shader->GetDescriptor();

		for (uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflector.GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC bufferDesc;
			constantBuffer->GetDesc(&bufferDesc);

			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflector.GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);

			std::string bufferName = bufferDesc.Name;

			if (descriptor.ContainsBuffer(bufferName) || bufferName._Starts_with("_cb") ||
				bindDesc.Type != D3D10_SIT_CBUFFER)
			{
				continue;
			}

			uint8_t* defaultBufferValue = new uint8_t[bufferDesc.Size];

			std::vector<ShaderDescriptor::VariableDescriptor> variables;
			variables.reserve(bufferDesc.Variables);

			for (uint32_t j = 0; j < bufferDesc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC variableDesc;
				variable->GetDesc(&variableDesc);

				std::string name        = std::string(variableDesc.Name);
				const uint64_t nameHash = std::hash<std::string>()(name);

				variables.emplace_back(ShaderDescriptor::VariableDescriptor{
					std::move(name),
					nameHash,
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

			const uint64_t nameHash = std::hash<std::string>()(bufferName);

			descriptor.AddBufferDescriptor(ShaderDescriptor::BufferDescriptor{
				std::move(bufferName),
				nameHash,
				(uint16_t) bindDesc.BindPoint,
				(uint16_t) bufferDesc.Size,
				defaultBufferValue,
				std::move(variables)
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
					std::string name        = std::string(desc.Name);
					const uint64_t nameHash = std::hash<std::string>()(name);

					descriptor.AddTextureDescriptor(ShaderDescriptor::TextureDescriptor{
						std::move(name),
						nameHash,
						(uint16_t) desc.BindPoint
					});
				}
			}
		}
	}
}
