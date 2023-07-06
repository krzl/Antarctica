#pragma once

#include "Asset.h"
#include "Types.h"

struct ShaderParams
{
	bool m_isDoubleSided    = false;
	bool m_blendingEnabled  = false;
	bool m_depthTestEnabled = true;
};

class Shader : public Asset
{
	typedef std::unique_ptr<Renderer::NativeShader, void(*)(Renderer::NativeShader*)> NativePtr;

public:

	[[nodiscard]] const Renderer::NativeShader* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	[[nodiscard]] Renderer::NativeShader* GetNativeObject()
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::NativeShader* nativePtr)
	{
		m_nativeObject = NativePtr(nativePtr, Renderer::Deleter);
	}

	[[nodiscard]] const std::string& GetPath() const
	{
		return m_path;
	}

	virtual std::unique_ptr<ShaderParams> GetShaderParams() { return std::unique_ptr<ShaderParams>(new ShaderParams); }


protected:

	bool Load(const std::string& path) override;

private:

	std::string m_path;

	NativePtr m_nativeObject = NativePtr(nullptr, Renderer::Deleter);
};