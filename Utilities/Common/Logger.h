#pragma once

enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger
{
public:

	static Logger& Get()
	{
		static Logger logger;
		return logger;
	}

	void SetLogLevel(const LogLevel level)
	{
		m_level = level;
	}

	void SetFilename(const std::string filename)
	{
		m_filename = filename;
	}

	template<typename... Args>
	void Log(LogLevel level, std::string category, std::string file, int line, const char* format, Args&&... args)
	{
		if (level < m_level)
		{
			return;
		}

		std::stringstream ss;
		ss << category << " - " << file << ":" << line << " - " << format;
		std::string full_message = FormatMessage(ss.str(), std::forward<Args>(args)...);

		std::cout << full_message << std::endl;

		std::ofstream outfile(m_filename, std::ios::app);
		outfile << full_message << std::endl;
		outfile.close();
	}

private:

	Logger() :
		m_level(DEBUG),
		m_filename("log.txt") {}

	LogLevel m_level;
	std::string m_filename;

	template<typename T, typename... Args>
	std::string FormatMessage(std::string format, T&& arg, Args&&... args)
	{
		std::stringstream ss;
		const size_t pos = format.find("{}");
		if (pos != std::string::npos)
		{
			ss << format.substr(0, pos) << arg;
			ss << FormatMessage(format.substr(pos + 2), std::forward<Args>(args)...);
		}
		else
		{
			ss << format;
		}
		return ss.str();
	}

	static std::string FormatMessage(std::string format)
	{
		return format;
	}
};

#define LOG(level, category, format, ...) Logger::Get().Log(level, category, __FILE__, __LINE__, format, ##__VA_ARGS__)
