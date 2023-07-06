#pragma once

#include "Asset.h"
#include "Types.h"

class ComputeShader : public Asset
{
	typedef std::unique_ptr<Renderer::NativeComputeShader, void(*)(Renderer::NativeComputeShader*)> NativePtr;

public:

	[[nodiscard]] const Renderer::NativeComputeShader* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	[[nodiscard]] Renderer::NativeComputeShader* GetNativeObject()
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::NativeComputeShader* nativePtr)
	{
		m_nativeObject = NativePtr(nativePtr, Renderer::Deleter);
	}

	[[nodiscard]] const std::string& GetPath() const
	{
		return m_path;
	}

	bool Load(const std::string& path) override;

private:

	std::string m_path;

	NativePtr m_nativeObject = NativePtr(nullptr, Renderer::Deleter);
};
