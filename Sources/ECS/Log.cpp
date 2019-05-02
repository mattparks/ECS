#include "Log.hpp"

#ifdef _WIN32
#include <windows.h>
#endif
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ecs
{
	// Log class Instance
	std::optional<Log::Impl> Log::m_log;

	void Log::Info(std::string const& message)
	{
		Get().Info(message);
	}

	void Log::Success(std::string const& message)
	{
		Get().Success(message);
	}

	void Log::Warning(std::string const& message)
	{
		Get().Warning(message);
	}

	void Log::Error(std::string const& message)
	{
		Get().Error(message);
	}

	void Log::EnableColors(bool enable)
	{
		Get().EnableColors(enable);
	}

	void Log::EnableDateTime(bool enable)
	{
		Get().EnableDateTime(enable);
	}

	Log::Impl& Log::Get()
	{
		if (!m_log.has_value())
		{
			m_log = Impl();
		}

		return m_log.value();
	}

	// Log::Impl static variables
	bool Log::Impl::m_isInit = false;
	bool Log::Impl::m_colorsAvailable = false;

	Log::Impl::Impl()
	{
		if (!m_isInit)
		{
			m_colorsAvailable = InitWindowsConsole();
			m_isInit = true;
		}

		m_enableColors = m_colorsAvailable;
	}

	void Log::Impl::Info(std::string const& message) const
	{
		Log("info", Color::Cyan, message);
	}

	void Log::Impl::Success(std::string const& message) const
	{
		Log("success", Color::Green, message);
	}

	void Log::Impl::Warning(std::string const& message) const
	{
		Log("warning", Color::Yellow, message);
	}

	void Log::Impl::Error(std::string const& message) const
	{
		Log("error", Color::Red, message);
	}

	void Log::Impl::EnableColors(bool enable)
	{
		m_enableColors = m_colorsAvailable && enable;
	}

	void Log::Impl::EnableDateTime(bool enable)
	{
		m_enableDateTime = enable;
	}

	void Log::Impl::Log(std::string const& header, Color headerColor, std::string const& message) const
	{
		PrintHeader(header, headerColor);

		if (m_enableDateTime)
		{
			PrintHeader(GetDate(), Color::Default);
		}

		PrintLine(message);
	}

	void Log::Impl::PrintHeader(std::string const& str, Color color) const
	{
		Print("[ ");

		Format(Style::Bold);
		Format(color);
		Print(str);

		Format(Style::Default);
		Print(" ] ");
	}

	void Log::Impl::PrintLine(std::string const& message) const
	{
		std::cout << message << std::endl;
	}

	void Log::Impl::Print(std::string const& message) const
	{
		std::cout << message;
	}

	void Log::Impl::Format(Style style) const
	{
		if (m_enableColors)
		{
			switch (style)
			{
			case Style::Bold:
				std::cout << "\033[1m";
				break;
			case Style::Underline:
				std::cout << "\033[4m";
				break;
			default:
				std::cout << "\033[0m";
			}
		}
	}

	void Log::Impl::Format(Color color) const
	{
		if (m_enableColors)
		{
			switch (color)
			{
			case Color::Cyan:
				std::cout << "\033[96m";
				break;
			case Color::Green:
				std::cout << "\033[92m";
				break;
			case Color::Yellow:
				std::cout << "\033[93m";
				break;
			case Color::Red:
				std::cout << "\033[91m";
				break;
			default:
				std::cout << "\033[39m";
			}
		}
	}

	std::string Log::Impl::GetDate() const
	{
		const auto now = std::chrono::system_clock::now();
		const auto time = std::chrono::system_clock::to_time_t(now);

		std::ostringstream ss;
		ss << std::put_time(std::localtime(&time), "%X");
		return ss.str();
	}

	bool Log::Impl::InitWindowsConsole()
	{
		// Windows only:
		// Enable console colors
#ifdef _WIN32

		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hOut == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		DWORD dwMode = 0;

		if (!GetConsoleMode(hOut, &dwMode))
		{
			return false;
		}

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
		// Fix build error with MinGW
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		if (!SetConsoleMode(hOut, dwMode))
		{
			return false;
		}
#endif

		return true;
	}
}
