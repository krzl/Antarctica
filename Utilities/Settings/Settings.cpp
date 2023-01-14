#include "stdafx.h"
#include "Settings.h"

#include "Common/StringHelper.h"

#include <algorithm>

bool Settings::LoadSettings(const std::string& filepath)
{
	m_filepath = filepath;

	std::ifstream file(filepath);
	if (!file.is_open())
		return false;

	std::string line;
	while (std::getline(file, line))
	{
		const auto split = StringHelper::Split(line, SETTINGS_TOKEN);
		if (split.size() >= 2)
		{
			SetValue(split[0], split[1]);
		}
	}

	return true;
}

bool Settings::SaveSettings()
{
	if (m_filepath.size() == 0)
		return false;

	std::ofstream file(m_filepath);
	if (!file.is_open())
		return false;

	std::vector<std::string> settingsKeys;
	for (auto& [key, _] : m_settings)
	{
		settingsKeys.push_back(key);
	}

	std::sort(settingsKeys.begin(), settingsKeys.end());

	for (auto& key : settingsKeys)
	{
		file << key << SETTINGS_TOKEN << m_settings.find(key)->second << std::endl;
	}

	return true;
}

std::optional<std::string> Settings::GetString(const std::string& key) const
{
	const auto it = m_settings.find(key);
	if (it == m_settings.end())
	{
		return std::optional<std::string>();
	}

	return it->second;
}

std::optional<int32_t> Settings::GetInt(const std::string& key) const
{
	const auto it = m_settings.find(key);
	if (it == m_settings.end())
	{
		return std::optional<int32_t>();
	}

	try
	{
		return std::stoi(it->second);
	}
	catch (std::exception)
	{
		return std::optional<int32_t>();
	}
}

std::optional<float> Settings::GetFloat(const std::string& key) const
{
	const auto it = m_settings.find(key);
	if (it == m_settings.end())
	{
		return std::optional<float>();
	}

	try
	{
		return std::stof(it->second);
	}
	catch (std::exception)
	{
		return std::optional<float>();
	}
}

void Settings::SetValue(const std::string& key, const std::string& value)
{
	m_settings[key] = value;
}

void Settings::SetValue(const std::string& key, const int32_t value)
{
	m_settings[key] = std::to_string(value);
}

void Settings::SetValue(const std::string& key, const float value)
{
	m_settings[key] = std::to_string(value);
}

void Settings::ResetValue(const std::string& key)
{
	m_settings.erase(key);
}

bool Settings::ContainsKey(const std::string& key) const
{
	return m_settings.find(key) != m_settings.end();
}
