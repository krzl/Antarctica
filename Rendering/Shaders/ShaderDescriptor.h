#pragma once

namespace Rendering
{
	struct ShaderDescriptor
	{
		struct TextureDescriptor
		{
			std::string m_name;
			uint64_t    m_nameHash;
			uint16_t    m_id;
		};

		struct VariableDescriptor
		{
			std::string m_name;
			uint64_t    m_nameHash;
			uint16_t    m_offset;
			uint16_t    m_byteSize;
		};

		struct BufferDescriptor
		{
			std::string                     m_name;
			uint64_t                        m_nameHash;
			uint16_t                        m_id;
			uint16_t                        m_bufferSize;
			uint8_t*                        m_defaultValue;
			std::vector<VariableDescriptor> m_variables;
		};

		void AddTextureDescriptor(TextureDescriptor&& descriptor);
		void AddBufferDescriptor(BufferDescriptor&& descriptor);

		bool ContainsBuffer(const std::string& name) const;
		bool ContainsTextureId(uint32_t id) const;

		void Clear();

		[[nodiscard]] const std::vector<TextureDescriptor>& GetTextures() const
		{
			return m_textures;
		}

		[[nodiscard]] const std::vector<BufferDescriptor>& GetBuffers() const
		{
			return m_buffers;
		}

		~ShaderDescriptor();

	private:

		std::vector<TextureDescriptor> m_textures;
		std::vector<BufferDescriptor>  m_buffers;
	};
}
