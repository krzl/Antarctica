#pragma once

#include "Asset.h"
#include "Shader.h"
#include "Texture.h"

class Shader;

enum RenderOrder
{
	TERRAIN      = 0,
	CHARACTER    = 100,
	TRANSPARENCY = 3000,
	UI = 5000
};

class Material : public Asset
{
public:

	struct MaterialVarData
	{
		uint8_t m_data[16];
	};

	explicit Material(const std::shared_ptr<Shader>& shader) :
		m_shaderParams{ shader->CreateDefaultShaderParams() },
		m_shader(shader) { }

	[[nodiscard]] const std::shared_ptr<Shader>& GetShader() const { return m_shader; }

	[[nodiscard]] float GetOrder() const { return m_order; }
	void SetOrder(const float order) { m_order = order; }

	[[nodiscard]] const std::unordered_map<uint64_t, std::shared_ptr<Texture>>& GetTextures() const { return m_textures; }
	void SetTexture(const std::string& name, std::shared_ptr<Texture> texture) { m_textures[std::hash<std::string>()(name)] = std::move(texture); }

	template<typename T, class = std::enable_if_t<sizeof(T) <= sizeof(MaterialVarData)>>
	void SetVariable(const std::string& name, T var)
	{
		MaterialVarData& varData = m_shaderVariables[std::hash<std::string>()(name)] = MaterialVarData();
		memcpy(varData.m_data, &var, sizeof(T));
	}

	[[nodiscard]] ShaderParams GetShaderParams() const { return m_shaderParams; }
	[[nodiscard]] ShaderParams& GetShaderParams() { return m_shaderParams; }
	void SetShaderParams(const ShaderParams shaderParams) { m_shaderParams = shaderParams; }

	[[nodiscard]] const std::unordered_map<uint64_t, MaterialVarData>& GetShaderVariables() const { return m_shaderVariables; }

protected:

	bool Load(const std::string& path) override { return true; }

private:

	std::unordered_map<uint64_t, std::shared_ptr<Texture>> m_textures;
	std::unordered_map<uint64_t, MaterialVarData> m_shaderVariables;

	ShaderParams m_shaderParams;

	std::shared_ptr<Shader> m_shader;
	float m_order = 0;
};
