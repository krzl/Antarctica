#pragma once

#include "Asset.h"
#include "Types.h"

class Texture : public Asset
{
	typedef std::unique_ptr<Renderer::NativeTexture, void(*)(Renderer::NativeTexture*)> NativePtr;

public:

	Texture() = default;

	Texture(const uint8_t* data, const int32_t width, const int32_t height, const int32_t channels, std::string path) :
		m_data(data),
		m_width(width),
		m_height(height),
		m_channels(channels),
		m_path(std::move(path)) { }

	[[nodiscard]] Renderer::NativeTexture* GetNativeObject()
	{
		return m_nativeObject.get();
	}


	[[nodiscard]] const Renderer::NativeTexture* GetNativeObject() const
	{
		return m_nativeObject.get();
	}

	void SetNativeObject(Renderer::NativeTexture* nativePtr)
	{
		m_nativeObject = NativePtr(nativePtr, Renderer::Deleter);
	}

	[[nodiscard]] const uint8_t*     GetData() const { return m_data; }
	[[nodiscard]] int32_t            GetWidth() const { return m_width; }
	[[nodiscard]] int32_t            GetHeight() const { return m_height; }
	[[nodiscard]] int32_t            GetChannels() const { return m_channels; }
	[[nodiscard]] const std::string& GetPath() const { return m_path; }

protected:

	bool Load(const std::string& path) override;

private:

	NativePtr m_nativeObject = NativePtr(nullptr, Renderer::Deleter);

	const uint8_t* m_data     = nullptr;
	int32_t        m_width    = 0;
	int32_t        m_height   = 0;
	int32_t        m_channels = 0;

	std::string m_path;
};
