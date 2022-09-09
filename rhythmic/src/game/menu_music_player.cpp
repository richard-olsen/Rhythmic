#include "menu_music_player.hpp"

#include "../container/song_container.hpp"

#include <random>

#include "../util/settings.hpp"

namespace Rhythmic
{
	typedef std::pair<const char *, const char *> SongLabel;

	SongLabel g_labels[3] = {
		{"",""},
		{"",""},
		{"",""}
	};

	MusicPlayerMenu::MusicPlayerMenu() :
		m_music(),
		m_songName(""),
		m_songArtist(""),
		m_allowPlaying(false)
	{}

	void MusicPlayerMenu::Start()
	{
		SettingValue value;
		value.type = SETTING_GAME_MENU_MUSIC;
		SettingsGet(&value);

		if (value.m_iValue)
			m_allowPlaying = true;
		else
			m_allowPlaying = false;
	}
	void MusicPlayerMenu::Stop()
	{
		m_allowPlaying = false;
	}
	void MusicPlayerMenu::Update()
	{
		if (SongContainer::IsScanning())
			m_allowPlaying = false;

		if (m_allowPlaying)
		{
			if (SongContainer::GetSongs().empty())
				return;

			if (!m_music.ContainsMusic() ||
				(m_music.ContainsMusic() && (m_music.GetDuration() + 0.025f >= m_music.GetSyncLength())))
				PickAndPlayMusic();
		}
		else
		{
			if (m_music.ContainsMusic())
			{
				m_music.Destroy();
				m_songName = "";
				m_songArtist = "";
			}
		}
	}
	void MusicPlayerMenu::PickAndPlayMusic()
	{
		std::random_device rDevice;
		std::mt19937 mtAlgorithm(rDevice());
		std::uniform_int_distribution<int> distribution(0, SongContainer::GetSongs().size() - 1);

		int songIndex = distribution(mtAlgorithm);

		ContainerSong &song = SongContainer::GetSong(songIndex);
		SettingValue settingValue;
		settingValue.type = SETTING_GAME_SONGLIST_DIRECTORY;
		SettingsGet(&settingValue);

		m_music.Create(AudioSystem::GetInstance(), settingValue.m_sValue + "/" + song.songFolder + "/");

		if (m_music.ContainsMusic())
		{
			m_music.Play();

			m_songName = song.name;
			m_songArtist = song.artist;
		}
		else
		{
			m_songName = "";
			m_songArtist = "";
		}
	}

	const std::string &MusicPlayerMenu::GetSongName()
	{
		return m_songName;
	}
	const std::string &MusicPlayerMenu::GetSongArtist()
	{
		return m_songArtist;
	}
}