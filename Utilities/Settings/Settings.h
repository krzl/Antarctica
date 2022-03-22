#pragma once

class Settings
{
public:
	
	bool LoadSettings(const std::string& filepath);
	bool SaveSettings();

	std::optional<std::string> GetString(const std::string& key) const;
	std::optional<int32_t> GetInt(const std::string& key) const;
	std::optional<float> GetFloat(const std::string& key) const;

	void SetValue(const std::string& key, const std::string& value);
	void SetValue(const std::string& key, int32_t value);
	void SetValue(const std::string& key, float value);
	
	void ResetValue(const std::string& key);
	bool ContainsKey(const std::string& key) const;
	
private:

	inline const static std::string SETTINGS_TOKEN = ": ";

	std::string m_filepath;

	std::map<std::string, std::string> m_settings;
};