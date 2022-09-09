#include "misc.hpp"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <Shlwapi.h>
#include <ShlObj_core.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <iostream>

namespace Rhythmic
{
	namespace Util
	{
		std::string g_pathExecutable;
		auto g_epoch = std::chrono::high_resolution_clock::now();
		bool g_closeGame;

		void Initialize()
		{
			g_closeGame = false;

			// Gets the actual execution path (not from where the executable is being executed, but where the executable is located)
			#ifdef WIN32

			char data[MAX_PATH];
			int dataSize = GetModuleFileName(NULL, data, MAX_PATH);
			std::string path = std::string(data, dataSize > 0 ? dataSize : 0);
			g_pathExecutable = path.substr(0, path.find_last_of('\\'));

			#else

			char data[PATH_MAX];
			ssize_t dataSize = readlink("/proc/self/exe", data, PATH_MAX);
			std::string path = std::string(data, dataSize > 0 ? dataSize : 0);
			g_pathExecutable = path.substr(0, path.find_last_of('/'));

			#endif
		}
		const std::string &GetExecutablePath()
		{
			return g_pathExecutable;
		}
		void SetExecutablePath(const std::string &str)
		{
			g_pathExecutable = str;
		}

		double GetTime()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - g_epoch).count() / 1000000000.0;
		}

		std::string RemoveQuotes(const std::string &string)
		{
			size_t firstQuote = string.find_first_of('\"');
			if (firstQuote != std::string::npos)
			{
				// Remove quotation marks
				size_t lastQuote = string.find_last_of('\"');

				if (lastQuote == std::string::npos)
					return string;

				return string.substr(firstQuote + 1, lastQuote - firstQuote - 1);
			}
			return string;
		}
		void Trim(std::string &string)
		{
			if (string.empty())
				return;

			// All space characters tested in isspace()
			// from https://www.tutorialspoint.com/c_standard_library/c_function_isspace.htm
			const static char *space = " \t\n\v\f\r";

			size_t first = string.find_first_not_of(space);
			size_t last = string.find_last_not_of(space);

			string = string.substr(first, last - first + 1);
		}
		void Split(const std::string &string, const std::string &delimiter, std::vector<std::string> &tokens)
		{
			if (string.empty())
				return;

			size_t first = 0;
			size_t last = string.find(delimiter, first);

			while (last != std::string::npos)
			{
				tokens.push_back(string.substr(first, last - first));
				first = last + delimiter.size();
				last = string.find(delimiter, first);
			}

			tokens.push_back(string.substr(first, last));
		}
		void ToLower(std::string &string)
		{
			if (string.empty())
				return;
			std::transform(string.begin(), string.end(), string.begin(), ::tolower);
		}
		int compareStringCaseInsensitive(std::string string1, std::string string2)
		{
			ToLower(string1);
			ToLower(string2);

			return string1.compare(string2);
		}

		void RequestGameToClose(bool failed)
		{
			g_closeGame = !failed;
		}
		bool HasGameRequestedToClose()
		{
			return g_closeGame;
		}

		const char *GetKeyName(int value)
		{
			const char *val = glfwGetKeyName(value, 0);

			if (val != NULL)
				return val;

			switch (value)
			{
				// Arrow Keys
			case GLFW_KEY_LEFT: return "Left";
			case GLFW_KEY_RIGHT: return "Right";
			case GLFW_KEY_UP: return "Up";
			case GLFW_KEY_DOWN: return "Down";

				// Function Keys
			case GLFW_KEY_F1: return "F1";
			case GLFW_KEY_F2: return "F2";
			case GLFW_KEY_F3: return "F3";
			case GLFW_KEY_F4: return "F4";
			case GLFW_KEY_F5: return "F5";
			case GLFW_KEY_F6: return "F6";
			case GLFW_KEY_F7: return "F7";
			case GLFW_KEY_F8: return "F8";
			case GLFW_KEY_F9: return "F9";
			case GLFW_KEY_F10: return "F10";
			case GLFW_KEY_F11: return "F11";
			case GLFW_KEY_F12: return "F12";
			case GLFW_KEY_F13: return "F13";
			case GLFW_KEY_F14: return "F14";
			case GLFW_KEY_F15: return "F15";
			case GLFW_KEY_F16: return "F16";
			case GLFW_KEY_F17: return "F17";
			case GLFW_KEY_F18: return "F18";
			case GLFW_KEY_F19: return "F19";
			case GLFW_KEY_F20: return "F20";
			case GLFW_KEY_F21: return "F21";
			case GLFW_KEY_F22: return "F22";
			case GLFW_KEY_F23: return "F23";
			case GLFW_KEY_F24: return "F24";
			case GLFW_KEY_F25: return "F25";

				// Above the Arrow Keys

			case GLFW_KEY_INSERT: return "Insert";
			case GLFW_KEY_HOME: return "Home";
			case GLFW_KEY_DELETE: return "Delete";
			case GLFW_KEY_END: return "End";
			case GLFW_KEY_PAGE_DOWN: return "Pg Down";
			case GLFW_KEY_PAGE_UP: return "Pg Up";

				// Misc

			case GLFW_KEY_ENTER: return "Enter";
			case GLFW_KEY_RIGHT_SHIFT: return "R Shift";
			case GLFW_KEY_LEFT_SHIFT: return "L Shift";
			case GLFW_KEY_RIGHT_ALT: return "R Alt";
			case GLFW_KEY_LEFT_ALT: return "L Alt";
			case GLFW_KEY_RIGHT_CONTROL: return "R Ctrl";
			case GLFW_KEY_LEFT_CONTROL: return "L Ctrl";
			case GLFW_KEY_CAPS_LOCK: return "Caps Lock";
			case GLFW_KEY_TAB: return "Tab";
			case GLFW_KEY_SPACE: return "Space";
			case GLFW_KEY_BACKSPACE: return "Backspace";
			case GLFW_KEY_ESCAPE: return "Escape";

			default:
				return "Unknown";
			}

			return val;
		}
	}
}