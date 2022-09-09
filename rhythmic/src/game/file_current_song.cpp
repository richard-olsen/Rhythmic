#include "file_current_song.hpp"

#include <fstream>
#include "../util/misc.hpp"

namespace Rhythmic
{
	namespace CurrentSongFile
	{
		void OutputSong(const std::string &name, const std::string &artist, const std::string &charter)
		{
			std::ofstream currentSong = std::ofstream(Util::GetExecutablePath() + "/current_song.txt");
			currentSong.clear();
			std::string lineSongName = std::string("Song: ") + name + "\n";
			currentSong.write(lineSongName.c_str(), lineSongName.size());
			std::string lineArtistName = std::string("Artist: ") + artist + "\n";
			currentSong.write(lineArtistName.c_str(), lineArtistName.size());
			std::string lineCharterName = std::string("Charter: ") + charter + "\n";
			currentSong.write(lineCharterName.c_str(), lineCharterName.size());
			currentSong.close();
		}
		void ClearSong()
		{
			std::ofstream stream = std::ofstream(Util::GetExecutablePath() + "/current_song.txt");
			stream.clear();
			stream.close();
		}
	}
}
