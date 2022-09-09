#ifndef MENU_MUSIC_PLAYER_HPP_
#define MENU_MUSIC_PLAYER_HPP_

#include "../util/singleton.hpp"

#include "music_player.hpp"

namespace Rhythmic
{
	class MusicPlayerMenu : public Singleton<MusicPlayerMenu>
	{
		friend class Singleton<MusicPlayerMenu>;
	public:
		void Start();
		void Stop();

		void Update();

		const std::string &GetSongName();
		const std::string &GetSongArtist();
	private:
		MusicPlayerMenu();

		void PickAndPlayMusic();

		MusicPlayer m_music;
		std::string m_songName;
		std::string m_songArtist;
		bool m_allowPlaying;
	};
}

#endif

