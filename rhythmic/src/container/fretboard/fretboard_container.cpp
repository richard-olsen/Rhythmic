#include "../song_container.hpp"

#include "fretboard.hpp"

#include <filesystem>

#include <wge/core/logging.hpp>
#include <wge/io/filesystem.hpp>

#include "../../util/misc.hpp"

#include <iostream>

#include "../custom_container.hpp"

namespace Rhythmic
{
	namespace FretboardContainer
	{
		typedef std::vector<Fretboard> FretList;
		static FretList g_fretboards;
		
		void Recache()
		{
			g_fretboards.clear();
			// Get directory to the fretboard folder in the game's path
			std::string dir = WGE::FileSystem::GetFileInDirectory("fretboards");

			// Find the default first
			std::string defaultFretboard = dir + FRETBOARD_DEFAULT_FILE;
			if (std::filesystem::is_regular_file(defaultFretboard)) // File exists
			{
				Fretboard fretboard;
				fretboard.m_name = "Default";
				fretboard.m_fileName = FRETBOARD_DEFAULT_FILE;

				g_fretboards.push_back(fretboard);
			}

			CustomContainer::Recache(dir, [](const std::filesystem::path &file)
				{
					std::string fileName = file.filename().string();
					if (Util::compareStringCaseInsensitive(fileName, "default.png") == 0)
						return;

					std::string ext = file.extension().string();
					if (Util::compareStringCaseInsensitive(ext, ".png") == 0 || Util::compareStringCaseInsensitive(ext, ".jpg") == 0)
					{
						std::string name = fileName.substr(0, fileName.find_last_of('.'));

						char first = toupper(name[0]);
						name[0] = first;

						Fretboard fretboard = { name, fileName };

						g_fretboards.push_back(fretboard);
					}
				});
			/*std::sort(g_fretboards.begin(), g_fretboards.end(), [](Fretboard first, Fretboard second) {
				return boost::ilexicographical_compare(first.m_name, second.m_name);
			});*/
		}

		Fretboard *GetFretboard(FretboardHandle index)
		{
			return &g_fretboards[index];
		}

		FretboardHandle FindFretboard(const std::string &name)
		{
			// Find the fretboard with the given name
			auto fret = std::find_if(g_fretboards.begin(), g_fretboards.end(), 
				[&name](const Fretboard &fretboard)
				{
					return Util::compareStringCaseInsensitive(fretboard.m_name, name) == 0;
				});

			// Return the handle
			if (fret != g_fretboards.end())
				return std::distance(g_fretboards.begin(), fret);

			return FRETBOARD_DEFAULT;
		}

		FretboardHandle GetNextValidHandle(FretboardHandle handle)
		{
			handle++;

			if (handle >= g_fretboards.size())
				return 0;

			return handle;
		}

		FretboardHandle GetPrevValidHandle(FretboardHandle handle)
		{
			handle--;

			if (handle < 0)
				return g_fretboards.size() - 1;

			return handle;
		}

		void Initialize()
		{
			Recache();
		}
		void Dispose()
		{
		}
	}
}

