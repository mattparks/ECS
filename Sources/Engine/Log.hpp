#pragma once

#include <mutex>
#include <string>
#include <memory>

namespace ecs
{
/**
 * @brief A logging class.
 */
class Log
{
public:
	/**
	 * Outputs a message into the console.
	 * @param string The string to output.
	 */
	static void Out(const std::string &string);

	/**
	 * Outputs a message into the console.
	 * @tparam Args The args types.
	 * @param format The format to output into.
	 * @param args The args to be added into the format.
	 */
	template<typename... Args>
	static void Out(const std::string &format, Args &&... args)
	{
		Out(StringFormat(format, std::forward<Args>(args)...));
	}

	/**
	 * Outputs a error into the console.
	 * @param string The string to output.
	 */
	static void Error(const std::string &string);

	/**
	 * Outputs a error into the console.
	 * @tparam Args The args types.
	 * @param format The format to output into.
	 * @param args The args to be added into the format.
	 */
	template<typename... Args>
	static void Error(const std::string &format, Args &&... args)
	{
		Error(StringFormat(format, std::forward<Args>(args)...));
	}

	/**
	 * Displays a popup menu.
	 * @param title The title.
	 * @param message The message.
	 */
	static void Popup(const std::string &title, const std::string &message);

private:
	template<typename... Args>
	static std::string StringFormat(const std::string &format, Args &&... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // Excludes the '\0'
	}
};
}
