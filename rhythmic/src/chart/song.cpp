#include "song.hpp"

#include <wge/core/logging.hpp>

#include "../menus/menu_system.hpp"

#include <filesystem>
#include "../util/misc.hpp"
#include "../util/ini_parser.hpp"

namespace Rhythmic
{
	Chart LoadChart(const std::string &songPath)
	{
		if (!std::filesystem::is_directory(songPath))
			return Chart();
		
		if (!std::filesystem::is_regular_file(songPath + "song.ini"))
			return Chart();

		Chart c;

		IniFile ini;
		IniSuccess success = IniRead(ini, songPath + "song.ini");
		if (success.success != 0)
		{
			LOG_ERROR("Song has invalid song.ini ({0}) ({1})!", songPath, success.errorMessage);
			return Chart();
		}
		else
		{
			auto sectionSong = ini.m_sections.find("song");
			if (sectionSong != ini.m_sections.end())
			{
				c.songData.artist = IniGetValue(sectionSong->second, SONG_KEY_ARTIST, SONG_UNKNOWN_ARTIST);
				c.songData.name = IniGetValue(sectionSong->second, SONG_KEY_NAME, SONG_UNKNOWN_SONG);
				c.songData.charter = IniGetValue(sectionSong->second, SONG_KEY_CHARTER, SONG_UNKNOWN_CHARTER);
				c.songData.supportsProDrums = Util::compareStringCaseInsensitive(IniGetValue(sectionSong->second, SONG_KEY_PRO_DRUMS, "False"), "True") == 0;
			}
			else
			{
				LOG_ERROR("Song section is missing in song.ini ({0})!", songPath);
				return Chart();
			}
		}

		int loadType = -1;
		std::string chart;
		if (std::filesystem::is_regular_file(songPath + "notes.chart"))
		{
			chart = songPath + "notes.chart";
			loadType = 0;
		}
		else if (std::filesystem::is_regular_file(songPath + "notes.mid"))
		{
			chart = songPath + "notes.mid";
			loadType = 1;
		}
		else if (std::filesystem::is_regular_file(songPath + "notes.midi"))
		{
			chart = songPath + "notes.midi";
			loadType = 1;
		}
		
		if (loadType == -1)
			return Chart(); // Finish (below code is unfinished) loadType = 0 for chart files loadType = 1 for midis

		if (loadType == 1)
			if (!LoadChartFromMidi(&c, chart)) {
				MenuSystem::GetInstance()->DisplayMessage("Failed to load chart at " + chart);
				return Chart();
			}
		if (loadType == 0)
			if (!LoadChartFromChart(&c, chart)) {
				MenuSystem::GetInstance()->DisplayMessage("Failed to load chart at " + chart);
				return Chart();
			}

			// Get the time of all BPMs and TS
		for (int i = 0; i < c.listBPM.size(); i++)
		{
			c.listBPM[i].time = GetAdjustedTimeFromPosition(c.listBPM[i].position, c);
		}
		for (int i = 0; i < c.listTS.size(); i++)
		{
			c.listTS[i].time = GetAdjustedTimeFromPosition(c.listTS[i].position, c);
		}

		//6 instruments
		//4 difficulties
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < c.spPhrases[i][j].size(); k++)
				{
					ChartStarpower &phrase = (c.spPhrases[i][j])[k];
					phrase.timeBegin = GetAdjustedTimeFromPosition(phrase.position, c);
					phrase.timeEnd = GetAdjustedTimeFromPosition(phrase.position + phrase.length, c);
				}
			}
		}

		float thing = ((4.0f * (float) c.songData.resolution) / 110.0f);
		c.songData.sustainScore = round(thing);

		if (c.listSection.empty())
		{
			// Utilize the last known note position with sustain length to figure it out
			unsigned int sectionsEvery = c.lastNotePosWithSustain / 10;

			for (int i = 0; i < 10; i++)
			{
				ChartSection currentSection;
				currentSection.position = sectionsEvery * i;
				int endingPosition = c.lastNotePosWithSustain;

				if (i < 9)
					endingPosition = sectionsEvery * (i + 1);

				currentSection.name = std::to_string((i + 1) * 10) + "%";
				currentSection.endPosition = endingPosition;
				// Calculate start and end time for practice mode
				currentSection.time = GetAdjustedTimeFromPosition(currentSection.position, c);
				currentSection.endTime = GetAdjustedTimeFromPosition(endingPosition, c);
				c.listSection.push_back(currentSection);
			}
		}
		else
		{
			for (int i = 0; i < c.listSection.size(); i++)
			{
				ChartSection &currentSection = c.listSection[i];
				int endingPosition = c.lastNotePosWithSustain;

				if ((i + 1) < c.listSection.size()) // More sections
					endingPosition = c.listSection[i + 1].position;

				currentSection.endPosition = endingPosition;
				// Calculate start and end time for practice mode
				currentSection.time = GetAdjustedTimeFromPosition(currentSection.position, c);
				currentSection.endTime = GetAdjustedTimeFromPosition(endingPosition, c);
			}
		}
		

		return c;
	}
}