#include "bg_image_container.hpp"

#include <filesystem>

#include "custom_container.hpp"

#include "../util/misc.hpp"

namespace Rhythmic
{
	namespace BGContainer
	{
		typedef std::vector<StageBackgroundIMG> BGImageList;
		static BGImageList g_backgrounds;

		void Initialize()
		{
			Recache();
		}
		void Dispose()
		{
			g_backgrounds.clear();
		}

		void Recache()
		{
			g_backgrounds.clear();
			// Get directory to the background image folder in the game's path
			std::string dir = WGE::FileSystem::GetFileInDirectory("custom/backgrounds/images");

			if (!std::filesystem::is_directory(WGE::FileSystem::GetFileInDirectory("custom/backgrounds")))
				std::filesystem::create_directories(WGE::FileSystem::GetFileInDirectory("custom/backgrounds"));

			g_backgrounds.push_back({ "Black", "" });

			CustomContainer::Recache(dir, [](const std::filesystem::path &file)
				{
					std::string fileName = file.filename().string();
					std::string ext = file.extension().string();
					if (Util::compareStringCaseInsensitive(ext, ".png") == 0 || Util::compareStringCaseInsensitive(ext, ".jpg") == 0)
					{
						std::string name = fileName.substr(0, fileName.find_last_of('.'));

						char first = toupper(name[0]);
						name[0] = first;

						StageBackgroundIMG background = { name, fileName };

						g_backgrounds.push_back(background);
					}
				});
		}

		StageBackgroundIMG *GetBackground(BackgroundIMGHandle index)
		{
			if (index < 0) index = 0;
			if (index >= g_backgrounds.size()) index = 0;

			return &g_backgrounds[index];
		}
		BackgroundIMGHandle FindBackground(const std::string &name)
		{
			// Find the background with the given name
			auto fret = std::find_if(g_backgrounds.begin(), g_backgrounds.end(),
				[&name](const StageBackgroundIMG &background)
				{
					return Util::compareStringCaseInsensitive(background.m_name, name) == 0;
				});

			// Return the handle
			if (fret != g_backgrounds.end())
				return std::distance(g_backgrounds.begin(), fret);

			return 0;
		}
		BackgroundIMGHandle GetNextValidHandle(BackgroundIMGHandle handle)
		{
			handle++;

			if (handle >= g_backgrounds.size())
				return 0;

			return handle;
		}
		BackgroundIMGHandle GetPrevValidHandle(BackgroundIMGHandle handle)
		{
			handle--;

			if (handle < 0)
				return g_backgrounds.size() - 1;

			return handle;
		}
	}
}