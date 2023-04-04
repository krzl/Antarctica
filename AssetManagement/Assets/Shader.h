#pragma once

#include "Asset.h"
#include "Types.h"

class Shader : public Asset
{
	typedef std::unique_ptr<Renderer::IShader, void(*)(Renderer::IShader*)> NativePtr;

public:

	explicit Shader(std::string path) :
		m_path(std::move(path)) { }

	[[nodiscard]] const Renderer::IShader* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	[[nodiscard]] Renderer::IShader* GetNativeObject()
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::IShader* nativePtr)
	{
		m_nativeObject = NativePtr(nativePtr, Renderer::Deleter);
	}

	[[nodiscard]] const std::string& GetPath() const
	{
		return m_path;
	}

private:

	std::string m_path;

	NativePtr m_nativeObject = NativePtr(nullptr, Renderer::Deleter);
};
