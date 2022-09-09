#ifndef SONG_CONTAINER_H_
#define SONG_CONTAINER_H_

#include <string>
#include <vector>

namespace Rhythmic
{
	constexpr int CS_GUITAR_EXPERT = (1 << 0);
	constexpr int CS_GUITAR_HARD   = (1 << 1);
	constexpr int CS_GUITAR_MEDIUM = (1 << 2);
	constexpr int CS_GUITAR_EASY   = (1 << 3);

	constexpr int CS_BASS_EXPERT = (1 << 4);
	constexpr int CS_BASS_HARD =   (1 << 5);
	constexpr int CS_BASS_MEDIUM = (1 << 6);
	constexpr int CS_BASS_EASY =   (1 << 7);

	struct ContainerSong
	{
		std::string songFolder;
		std::string name;
		std::string artist;
		std::string chartFile;
		int64_t field;
	};
	namespace SongContainer
	{
		void Initialize();
		void Dispose();

		bool ReadCache();
		void CacheSongs();

		bool IsScanning();

		unsigned int	Size();
		ContainerSong	&GetSong(unsigned int index);
		std::vector<ContainerSong> &GetSongs();
	}
}

#endif
