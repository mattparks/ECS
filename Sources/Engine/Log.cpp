#include "Log.hpp"

#include <fstream>
#if defined(ACID_BUILD_WINDOWS)
#include <Windows.h>
#endif

namespace ecs
{
void Log::Out(const std::string &string)
{
	fprintf(stdout, "%s", string.c_str());
}

void Log::Error(const std::string &string)
{
	fprintf(stderr, "%s", string.c_str());
}

void Log::Popup(const std::string &title, const std::string &message)
{
#if defined(ACID_BUILD_WINDOWS)
	MessageBox(nullptr, message.c_str(), title.c_str(), 0);
#endif
}
}
