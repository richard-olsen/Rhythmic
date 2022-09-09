#include "custom_container.hpp"
#include <assert.h>

namespace Rhythmic
{
	/*
	CustomContainer is a skeleton for ALL custom content containers
	such as Background Images, Fretboards, etc.
	*/
	namespace CustomContainer
	{
		void Recache(const std::string &directory, std::function<void(const std::filesystem::path &)> rules)
		{
			assert(rules && "Need a set of rules");

			// Check to see if it exists, create if it doesn't exist
			if (!std::filesystem::is_directory(directory))
				std::filesystem::create_directory(directory);

			for (std::filesystem::directory_iterator it(directory); it != std::filesystem::directory_iterator(); it++)
			{
				rules(it->path());
			}
		}
	}
}
