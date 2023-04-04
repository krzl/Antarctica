#pragma once

#include "Asset.h"
#include "Types.h"

class ComputeShader : public Asset
{
	typedef std::unique_ptr<Renderer::IComputeShader, void(*)(Renderer::IComputeShader*)> NativePtr;

public:

	explicit ComputeShader(std::string path) :
		m_path(std::move(path)) { }

	[[nodiscard]] const Renderer::IComputeShader* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	[[nodiscard]] Renderer::IComputeShader* GetNativeObject()
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::IComputeShader* nativePtr)
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
