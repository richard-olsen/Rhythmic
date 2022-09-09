#include "song_container.hpp"

#include <filesystem>

#include <fstream>

#include <wge/core/logging.hpp>

#include "../chart/song.hpp"

#include "../util/misc.hpp"
#include "../util/settings.hpp"
#include "../util/ini_parser.hpp"

#include <iostream>
#include <vector>

namespace Rhythmic
{
	namespace SongContainer
	{
		std::vector<ContainerSong> g_songs;
		std::vector<ContainerSong> g_invalidSongs;

		//void AttemptToFixInvalidSongs()

		bool g_isScanning;

		void ReadSongsToArray(std::ifstream &stream)
		{
			SettingValue value;
			value.type = SETTING_GAME_SONGLIST_DIRECTORY;
			SettingsGet(&value);
			std::string songPath = value.m_sValue;
			if (songPath.empty()) // Is song path empty?
				return;

			char lastSongPathChar = songPath[songPath.size() - 1]; // Last char of song path
			if (lastSongPathChar != '/' && lastSongPathChar != '\\')
				songPath.append("/");
			
			std::string song;
			while (stream.good()) // Keep going if stream is alive
			{
				std::getline(stream, song);
				if (song.empty())
					continue;

				ContainerSong song_container;
				song_container.songFolder = song;

				if (std::filesystem::is_regular_file(songPath + song + "/notes.chart"))
					song_container.chartFile = "notes.chart";
				else if (std::filesystem::is_regular_file(songPath + song + "/notes.mid"))
					song_container.chartFile = "notes.mid";
				else // Truely invalid. No notes to load, the invalid list isn't even an option for it
					continue;
				

				if (!std::filesystem::is_regular_file(songPath + song + "/song.ini"))
				{	// If there is no song.ini file, the song is invalid! Add it to the invalid list and allow
					// 		Rhythmic to attempt to fix it
					g_invalidSongs.push_back(song_container);
					continue;
				}

				IniFile ini;
				IniSuccess success = IniRead(ini, songPath + song + "/song.ini");
				if (success.success != 0)
				{
					song_container.songFolder += std::string(" (invalid song.ini: \"") + success.errorMessage + std::string("\")");
					break;
				}
				else
				{
					auto sectionSong = ini.m_sections.find("song");
					if (sectionSong != ini.m_sections.end())
					{
						song_container.artist = IniGetValue(sectionSong->second, SONG_KEY_ARTIST, SONG_UNKNOWN_ARTIST);
						song_container.name = IniGetValue(sectionSong->second, SONG_KEY_NAME, SONG_UNKNOWN_SONG);
					}
				}
				// THIS IS BAD RIGHT HERE, AN EXCEPTION CAN BE THROWN AND IT'S NOT BEING CAUGHT. THIS WILL CAUSE A CRASH!
				//boost::property_tree::iptree song_ini; // The keys need to be case insensitive
				//boost::property_tree::read_ini(songPath + song + "/song.ini", song_ini);
				//
				//song_container.name = song_ini.get(SONG_KEY_NAME, SONG_UNKNOWN_SONG);
				//song_container.artist = song_ini.get(SONG_KEY_ARTIST, SONG_UNKNOWN_ARTIST);

				g_songs.push_back(song_container);
			}
		}

		void Initialize()
		{
			std::ifstream song_cache(Util::GetExecutablePath() + "/song_cache.txt");
			if (song_cache.is_open())
			{
				ReadSongsToArray(song_cache);
			}
			else
			{
				song_cache.close();
				CacheSongs();
			}
			if (song_cache.is_open())
				song_cache.close();
			
			if (g_invalidSongs.size() > 0)
			{
				std::ofstream song_cache_invalid(Util::GetExecutablePath() + "/song_invalid.txt");
				song_cache_invalid.clear();

				for (int i = 0; i < g_invalidSongs.size(); i++)
					song_cache_invalid << g_invalidSongs[i].songFolder << std::endl;
				
				song_cache_invalid.close();
			}

			g_isScanning = false;
		}
		void Dispose()
		{

		}

		bool IsScanning()
		{
			return g_isScanning;
		}

		bool ReadCache()
		{
			std::ifstream song_cache(Util::GetExecutablePath() + "/song_cache.txt");
			if (song_cache.is_open())
				ReadSongsToArray(song_cache);
			else
			{
				song_cache.close();
				return false;
			}

			song_cache.close();

			if (g_invalidSongs.size() > 0)
			{
				std::ofstream song_cache_invalid(Util::GetExecutablePath() + "/song_invalid.txt");
				song_cache_invalid.clear();

				for (int i = 0; i < g_invalidSongs.size(); i++)
					song_cache_invalid << g_invalidSongs[i].songFolder << std::endl;

				song_cache_invalid.close();
			}

			if (g_songs.empty())
				return false;
			return true;
		}

		void CacheDirectory(const std::string &directory, const std::string &song_path, std::ofstream &song_cache)
		{
			bool maySaveSong = directory.size() > song_path.size();

			bool foundIni = false;
			bool iniMissing = true;
			bool isChartFoundChartFile = false;
			std::string iniMessage = "";

			ContainerSong song;
			if (maySaveSong)
				song.songFolder = directory.substr(song_path.size());//

			/*
			Iterates through each directory within song_path and caches the directory.
			*/
			for (std::filesystem::directory_iterator it(directory); it != std::filesystem::directory_iterator(); it++)
			{
				std::string path = it->path().string(); // Current path (as string)

				if (std::filesystem::is_directory(path))
				{
					CacheDirectory(path, song_path, song_cache);
				}
				else if (std::filesystem::is_regular_file(path))
				{
					//std::string requiredChart = path + "/notes.chart";
					std::string file = it->path().filename().string();

					if (Util::compareStringCaseInsensitive(file, "notes.chart") == 0) // Check for chart file
					{
						song.chartFile = file;
						isChartFoundChartFile = true;
					}
					if (Util::compareStringCaseInsensitive(file, "notes.mid") == 0) // Check for mid file
						song.chartFile = file;
					if (Util::compareStringCaseInsensitive(file, "song.ini") == 0) // Check for ini
					{
						iniMissing = false;
						IniFile ini; // Song Ini Object
						IniSuccess success = IniRead(ini, path);
						if (success.success != 0) // Check if ini file can be read/exists.
						{
							iniMessage = std::string(" (invalid song.ini: \"") + success.errorMessage + std::string("\")");
							break;
						}
						else
						{
							auto sectionSong = ini.m_sections.find("song");
							if (sectionSong != ini.m_sections.end())
							{
								song.artist = IniGetValue(sectionSong->second, SONG_KEY_ARTIST, SONG_UNKNOWN_ARTIST);
								song.name = IniGetValue(sectionSong->second, SONG_KEY_NAME, SONG_UNKNOWN_SONG);

								foundIni = true;
							}
						}
					}
				}
			}

			if (!iniMessage.empty())
				LOG_WARNING("Invalid Song INI: {0}", iniMessage);

			if (maySaveSong)
			{
				if (song.chartFile.empty())
					return;
				
				if (foundIni)
				{
					g_songs.push_back(song);
					song_cache << song.songFolder << std::endl;
				}
				else
				{
					if (iniMissing && isChartFoundChartFile) // If INI doesn't exist and the chart file found is notes.chart
					{
						Chart dataChart;
						LoadChartFromChart(&dataChart, directory + "/" + song.chartFile, true, true);

						IniFile ini;
						ini.m_sections.insert(std::pair<std::string, IniMap>("song", IniMap()));
						ini.m_sections["song"].insert(IniEntry(SONG_KEY_NAME, dataChart.songData.name));
						ini.m_sections["song"].insert(IniEntry(SONG_KEY_ARTIST, dataChart.songData.artist));
						ini.m_sections["song"].insert(IniEntry(SONG_KEY_CHARTER, dataChart.songData.charter));

						IniWrite(ini, directory + "/song.ini");
						song_cache << song.songFolder << std::endl;
					}
					else
					{
						song.songFolder += iniMessage;
						g_invalidSongs.push_back(song);
					}
				}
			}
		}

		void CacheSongs()
		{
			g_isScanning = true;

			SettingValue value; // New value
			value.type = SETTING_GAME_SONGLIST_DIRECTORY; // Set type to SONGLIST_DIRECTORY
			SettingsGet(&value); // Gets the current value of SONGLIST_DIRECTORY by passing in a reference to the value variable.
			std::string songListPath = value.m_sValue; // Path of Song List Directory

			if (songListPath.empty())
				return;

			char lastDirPathChar = songListPath[songListPath.size() - 1]; // Get last character of songListPath

			if (lastDirPathChar != '/' && lastDirPathChar != '\\')
				songListPath.append("/"); // Append / to end of path

			LOG_INFO("Scanning for Songs in: {0}...", songListPath);
			try
			{
				if (std::filesystem::is_directory(songListPath))
				{
					std::ofstream song_cache(Util::GetExecutablePath() + "/song_cache.txt");
					song_cache.clear();
					g_songs.clear();
					g_invalidSongs.clear();

					CacheDirectory(songListPath, songListPath, song_cache);

					song_cache.close();

					if (g_invalidSongs.size() > 0)
					{
						std::ofstream song_invalid(Util::GetExecutablePath() + "/song_invalid.txt");
						for (int i = 0; i < g_invalidSongs.size(); i++)
						{
							song_invalid << g_invalidSongs[i].songFolder << std::endl;
						}
						song_invalid.close();
					}
				}
				else
				{
					std::filesystem::create_directory(songListPath);
					std::ofstream song_cache(Util::GetExecutablePath() + "/song_cache.txt");
					g_songs.clear();
					g_invalidSongs.clear();
					song_cache.clear();
					song_cache.close();
				}
			}
			catch (std::filesystem::filesystem_error e)
			{
				LOG_WARNING("Caching failed! {0}", e.what());
			}

			g_isScanning = false;
			return;
		}

		unsigned int Size()
		{
			return g_songs.size();
		}

		std::vector<ContainerSong> &GetSongs() {
			return g_songs;
		}

		ContainerSong &GetSong(unsigned int index)
		{
			assert(index < g_songs.size());
			return g_songs[index];
		}
	}
}

