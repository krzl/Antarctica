#pragma once

#include "Asset.h"
#include "Types.h"

class ComputeShader : public Asset
{
	typedef std::unique_ptr<Rendering::NativeComputeShader, void(*)(Rendering::NativeComputeShader*)> NativePtr;

public:

	[[nodiscard]] const Rendering::NativeComputeShader* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	[[nodiscard]] Rendering::NativeComputeShader* GetNativeObject()
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Rendering::NativeComputeShader* nativePtr)
	{
		m_nativeObject = NativePtr(nativePtr, Rendering::Deleter);
	}

	[[nodiscard]] const std::string& GetPath() const
	{
		return m_path;
	}

	bool Load(const std::string& path) override;

private:

	std::string m_path;

	NativePtr m_nativeObject = NativePtr(nullptr, Rendering::Deleter);
};
