#ifndef FILE_CURRENT_SONG_H_
#define FILE_CURRENT_SONG_H_

#include <string>

namespace Rhythmic
{
	namespace CurrentSongFile
	{
		void OutputSong(const std::string &name, const std::string &artist, const std::string &charter);
		void ClearSong();
	}
}

#endif

