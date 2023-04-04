#pragma once

namespace Renderer
{
	struct ShaderDescriptor
	{
		struct TextureDescriptor
		{
			const std::string m_name;
			const uint16_t    m_id;
		};

		struct VariableDescriptor
		{
			const std::string m_name;
			const uint16_t    m_bufferId;
			const uint16_t    m_offset;
			const uint16_t    m_byteSize;
		};

		struct BufferDescriptor
		{
			const std::string m_name;
			const uint16_t    m_bufferSize;
			const uint8_t*    m_defaultValue;
		};

		void AddTextureDescriptor(TextureDescriptor&& descriptor);
		void AddVariableDescriptor(VariableDescriptor&& descriptor);
		void AddBufferDescriptor(BufferDescriptor&& descriptor);

		bool ContainsBuffer(const std::string& name) const;
		bool ContainsTextureId(uint32_t id) const;

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

		std::vector<TextureDescriptor>  m_textures;
		std::vector<VariableDescriptor> m_variables;
		std::vector<BufferDescriptor>   m_buffers;
	};
}
