#pragma once
#include <d3d12shader.h>

namespace Renderer
{
	struct ShaderDescriptor
	{
		struct TextureDescriptor
		{
			const std::string m_name;
			const uint16_t m_id;
		};

		struct VariableDescriptor
		{
			const std::string m_name;
			const uint16_t m_bufferId;
			const uint16_t m_offset;
			const uint16_t m_byteSize;
		};

		struct BufferDescriptor
		{
			const uint16_t m_id;
			const uint16_t m_bufferSize;
			const uint8_t* m_defaultValue;
		};

		void AddFromReflector(ID3D12ShaderReflection* reflector, const D3D12_SHADER_DESC& descriptor);
		void Clear();

		[[nodiscard]] const std::vector<TextureDescriptor>& GetTextures() const
		{
			return m_textures;
		}

		[[nodiscard]] const std::vector<VariableDescriptor>& GetVariables() const
		{
			return m_variables;
		}
		
		[[nodiscard]] const std::vector<BufferDescriptor>& GetBuffers() const
		{
			return m_buffers;
		}

		~ShaderDescriptor();
		
	private:

		std::vector<TextureDescriptor> m_textures;
		std::vector<VariableDescriptor> m_variables;
		std::vector<BufferDescriptor> m_buffers;
	};
}
