#pragma once

class Asset
{
	friend class AssetManager;
	
public:

	virtual ~Asset() = default;

protected:

	Asset() = default;

	virtual bool Load(const std::string& path) = 0;
};
