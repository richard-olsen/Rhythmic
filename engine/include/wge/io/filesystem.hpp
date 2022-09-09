#ifndef WGE_CORE_FILESYSTEM_HPP_
#define WGE_CORE_FILESYSTEM_HPP_

#include <string>

namespace WGE
{
	namespace FileSystem
	{
		/*
		Initialize the file system allowing GetExecutablePath to return the correct
		value, and other stuff File System will end up doing
		*/
		void InitializeFileSystem();

		/*
		Returns the path of the executable by default, unless set by SetExecutablePath
		*/
		const std::string &GetExecutablePath();
		/*
		Tells the engine the game's executable is in another location. Useful for debugging
		*/
		void SetExecutablePath(const std::string &str);

		/*
		Retrieves the path to a file within the executable path. This DOES NOT check
		to see if the file exists
		*/
		std::string GetFileInDirectory(const std::string &file);
	}
}

#endif

