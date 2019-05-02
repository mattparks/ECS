#pragma once

#include <optional>
#include <string>

namespace ecs
{
class Log
{
public:
	Log() = delete;

	// Print info message
	static void Info(std::string const &message);

	// Print success message
	static void Success(std::string const &message);

	// Print warning message
	static void Warning(std::string const &message);

	// Print error message
	static void Error(std::string const &message);

	// Enable or disable colors
	static void EnableColors(bool enable = true);

	// Enable or disable date time
	static void EnableDateTime(bool enable = true);

private:
	class Impl;

	// Get Log class instance
	static Impl &Get();

	// Log class instance
	static std::optional<Impl> m_log;
};

class Log::Impl
{
public:
	Impl();

	~Impl() = default;

	Impl(Impl const &) = delete;

	Impl(Impl &&) noexcept = default;

	Impl &operator=(Impl const &) = delete;

	Impl &operator=(Impl &&) noexcept = default;

	// Print info message
	void Info(std::string const &message) const;

	// Print success message
	void Success(std::string const &message) const;

	// Print warning message
	void Warning(std::string const &message) const;

	// Print error message
	void Error(std::string const &message) const;

	// Enable or disable colors
	void EnableColors(bool enable = true);

	// Enable or disable date time
	void EnableDateTime(bool enable = true);

private:
	enum class Color
	{
		Default, Cyan, Green, Yellow, Red
	};

	enum class Style
	{
		Default, Bold, Underline
	};

	// Log a message
	void Log(std::string const &header, Color headerColor, std::string const &message) const;

	// Print message header to stdout
	void PrintHeader(std::string const &str, Color color) const;

	// Print text to stdout followed by a new line
	void PrintLine(std::string const &str) const;

	// Print text to stdout
	void Print(std::string const &str) const;

	// Change console text style
	void Format(Style style) const;

	// Change console text color
	void Format(Color color) const;

	// Get current date
	std::string GetDate() const;

	// Configure colors for Windows 10 console
	bool InitWindowsConsole();

	// Are the colors enabled
	bool m_enableColors{ true };

	// Is the date enabled
	bool m_enableDateTime{ true };

	// Is the console configured
	static bool m_isInit;

	// Are the colors available
	static bool m_colorsAvailable;
};
}
