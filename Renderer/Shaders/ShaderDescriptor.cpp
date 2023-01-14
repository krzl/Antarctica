#include "stdafx.h"
#include "ShaderDescriptor.h"

namespace Renderer
{
	static bool IsElementIdPresent(const std::vector<ShaderDescriptor::TextureDescriptor>& list, const uint8_t id)
	{
		for (const ShaderDescriptor::TextureDescriptor& element : list)
		{
			if (element.m_id == id)
			{
				return true;
			}
		}

		return false;
	}
	
	void ShaderDescriptor::AddFromReflector(ID3D12ShaderReflection* reflector, const D3D12_SHADER_DESC& descriptor)
	{
		for (uint32_t i = 0; i < descriptor.ConstantBuffers; ++i)
		{
			ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflector->GetConstantBufferByIndex(0);
			D3D12_SHADER_BUFFER_DESC bufferDesc;
			constantBuffer->GetDesc(&bufferDesc);

			uint8_t* defaultBufferValue = new uint8_t[bufferDesc.Size];

			for (uint32_t j = 0; j < bufferDesc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC variableDesc;
				variable->GetDesc(&variableDesc);

				m_variables.emplace_back(VariableDescriptor{
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

			m_buffers.emplace_back(BufferDescriptor{
									   (uint16_t) i,
									   (uint16_t) bufferDesc.Size,
									   defaultBufferValue
								   });
		}
		
		for (uint32_t i = 0; i < descriptor.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC desc;
			reflector->GetResourceBindingDesc(i, &desc);

			if (desc.Type == D3D_SIT_TEXTURE)
			{
				if (!IsElementIdPresent(m_textures, desc.BindPoint))
				{
					m_textures.emplace_back(TextureDescriptor{ std::string(desc.Name), (uint16_t) desc.BindPoint });
				}
			}
		}
	}
	
	void ShaderDescriptor::Clear()
	{
		m_textures.clear();
	}
	
	ShaderDescriptor::~ShaderDescriptor()
	{
		for (BufferDescriptor& bufferInfo : m_buffers)
		{
			delete[] bufferInfo.m_defaultValue;
		}
	}
}
