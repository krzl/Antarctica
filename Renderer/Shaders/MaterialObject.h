#pragma once

#include "ShaderObject.h"

namespace Renderer
{
	class MaterialObject
	{
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
	};
}
