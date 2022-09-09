#include <wge/io/filesystem.hpp>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace WGE
{
	namespace FileSystem
	{
		std::string g_executablePath;

		/*
		Sets g_executablePath to the directory where the executable lives since
		all game content should live inside this directory
		*/
		void InitializeExecutablePath()
		{
#ifdef WIN32

			char data[MAX_PATH];
			int dataSize = GetModuleFileName(NULL, data, MAX_PATH);
			std::string path = std::string(data, dataSize > 0 ? dataSize : 0);
			g_executablePath = path.substr(0, path.find_last_of('\\'));

#else

			char data[PATH_MAX];
			ssize_t dataSize = readlink("/proc/self/exe", data, PATH_MAX);
			std::string path = std::string(data, dataSize > 0 ? dataSize : 0);
			g_executablePath = path.substr(0, path.find_last_of('/'));

#endif
		}

		void InitializeFileSystem()
		{
			// Put more initialization here if neccessary
			InitializeExecutablePath();
		}

		const std::string &GetExecutablePath()
		{
			return g_executablePath;
		}

		void SetExecutablePath(const std::string &str)
		{
			g_executablePath = str;
		}


		std::string GetFileInDirectory(const std::string &file)
		{
			return g_executablePath + "/" + file;
		}
	}
}