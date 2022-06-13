#pragma once

class Logger
{
public:

	template<typename... Args>
	static void Log(const std::string& text, Args ... args)
	{
		const uint32_t size_s = std::snprintf(nullptr, 0, text.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0)
		{
			throw std::runtime_error("Error during formatting.");
		}
		const size_t size = static_cast<size_t>(size_s);
		const std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, text.c_str(), args ...);
		std::cout << std::string(buf.get(), buf.get() + size - 1) << std::endl;
	}
};
