#pragma once

#include "ShaderObject.h"
#include "../AssetManagement/Assets/Texture.h"

class Material;

namespace Renderer
{
	class MaterialObject
	{
		friend class Material;
		
	public:

		explicit MaterialObject(ShaderObject& shader) :
			m_shader(shader)
		{
		}

		[[nodiscard]] const ShaderObject& GetShaderObject() const
		{
			return m_shader;
		}
		
		void UpdateAndBind() const;

	private:

		ShaderObject& m_shader;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
	};
}
