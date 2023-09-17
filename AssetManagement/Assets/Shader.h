#pragma once

#include "Asset.h"
#include "Types.h"

struct ShaderParams
{
	uint8_t m_isDoubleSided : 1;
	uint8_t m_blendingEnabled : 1;
	uint8_t m_depthTestEnabled : 1;
	uint8_t m_isWireframe : 1;

	ShaderParams()
	{
		m_isDoubleSided    = false;
		m_blendingEnabled  = false;
		m_depthTestEnabled = true;
		m_isWireframe      = false;
	}


	// ReSharper disable once CppNonExplicitConversionOperator
	operator uint8_t() const
	{
		return *reinterpret_cast<const uint8_t*>(this);
	}
};

class Shader : public Asset
{
	typedef std::unique_ptr<Rendering::NativeShader, void(*)(Rendering::NativeShader*)> NativePtr;

public:

	[[nodiscard]] const Rendering::NativeShader* GetNativeObject() const { return m_nativeObject.get(); }
	[[nodiscard]] Rendering::NativeShader* GetNativeObject() { return m_nativeObject.get(); }
	void SetNativeObject(Rendering::NativeShader* nativePtr) { m_nativeObject = NativePtr(nativePtr, Rendering::Deleter); }

	[[nodiscard]] const std::string& GetPath() const { return m_path; }

	virtual ShaderParams CreateDefaultShaderParams() { return ShaderParams(); }

protected:

	bool Load(const std::string& path) override;

	std::string m_path;

private:

	NativePtr m_nativeObject = NativePtr(nullptr, Rendering::Deleter);
};
