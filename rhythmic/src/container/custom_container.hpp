#ifndef RHYTHMIC_CUSTOM_CONTAINER_HPP
#define RHYTHMIC_CUSTOM_CONTAINER_HPP

#include <string>
#include <vector>
#include <functional>

#include <filesystem>

#include <wge/io/filesystem.hpp>

// Should probably refactor the names here

namespace Rhythmic
{
	/*
	CustomContainer is a skeleton for ALL custom content containers
	such as Background Images, Fretboards, etc.
	*/
	namespace CustomContainer
	{
		/*
		Rescans the given directory for content. Will scan every file inside
		the directory and pass it into rules. Rules determines what it'll do
		with the given file or directory

		directory - The directory to scan in
		rules - The custom set of rules to really pick into it (CAN NOT BE NULL)
		*/
		void Recache(const std::string &directory, std::function<void(const std::filesystem::path &)> rules);
	}
}

#endif

