#include "music_player.hpp"

#include <wge/core/logging.hpp>


namespace Rhythmic
{
	MusicPlayer::MusicPlayer() :
		m_audioSystem(0),
		m_audioDrums(0),
		m_audioDrums1(0),
		m_audioDrums2(0),
		m_audioDrums3(0),
		m_audioDrums4(0),
		m_audioKeys(0),
		m_audioGuitar(0),
		m_audioRhythm(0),
		m_audioSong(0),
		m_audioLyrics(0),
		m_audioSync(0),
		m_channelGroup(0)
	{ }
	MusicPlayer::~MusicPlayer()
	{ }

	void MusicPlayer::Create(AudioSystem *system, const std::string &songPath, bool isSync)
	{
		m_audioSystem = system;

		loadAudio(&m_audioDrums, songPath + "drums");
		loadAudio(&m_audioDrums1, songPath + "drums_1");
		loadAudio(&m_audioDrums2, songPath + "drums_2");
		loadAudio(&m_audioDrums3, songPath + "drums_3");
		loadAudio(&m_audioDrums4, songPath + "drums_4");
		loadAudio(&m_audioKeys, songPath + "keys");
		loadAudio(&m_audioGuitar, songPath + "guitar");
		loadAudio(&m_audioRhythm, songPath + "rhythm");
		loadAudio(&m_audioSong, songPath + "song");
		loadAudio(&m_audioLyrics, songPath + "vocals");

		m_audioSync = m_audioGuitar;
	}
	void MusicPlayer::Destroy()
	{
		freeAudio(&m_audioDrums);
		freeAudio(&m_audioDrums1);
		freeAudio(&m_audioDrums2);
		freeAudio(&m_audioDrums3);
		freeAudio(&m_audioDrums4);
		freeAudio(&m_audioKeys);
		freeAudio(&m_audioGuitar);
		freeAudio(&m_audioRhythm);
		freeAudio(&m_audioSong);
		freeAudio(&m_audioLyrics);

		m_audioSync = 0;
	}

	void MusicPlayer::Play()
	{
		playAudio(m_audioDrums);
		playAudio(m_audioDrums1);
		playAudio(m_audioDrums2);
		playAudio(m_audioDrums3);
		playAudio(m_audioDrums4);
		playAudio(m_audioKeys);
		playAudio(m_audioGuitar);
		playAudio(m_audioRhythm);
		playAudio(m_audioSong);
		playAudio(m_audioLyrics);
	}
	void MusicPlayer::Pause()
	{
		pauseAudio(m_audioDrums);
		pauseAudio(m_audioDrums1);
		pauseAudio(m_audioDrums2);
		pauseAudio(m_audioDrums3);
		pauseAudio(m_audioDrums4);
		pauseAudio(m_audioKeys);
		pauseAudio(m_audioGuitar);
		pauseAudio(m_audioRhythm);
		pauseAudio(m_audioSong);
		pauseAudio(m_audioLyrics);
	}

	void MusicPlayer::SetLocalVolume(float volume)
	{
	}

	double MusicPlayer::GetSyncLength()
	{
		if (!m_audioSync)
			return 0;
		return m_audioSync->GetLengthInSeconds();
	}

	void MusicPlayer::SetDuration(double duration)
	{
	}
	double MusicPlayer::GetDuration()
	{
		if (!m_audioSync)
			return 0;
		return m_audioSync->GetPlaybackDuration();
	}

	void MusicPlayer::SetPlaybackSpeed(float speed)
	{
		m_playbackSpeed = speed;
	}

	float MusicPlayer::GetPlaybackSpeed()
	{
		return m_playbackSpeed;
	}

	bool MusicPlayer::IsPlaying()
	{
		if (!m_audioSync)
			return false;

		return m_audioSync->IsPlaying();
	}

	bool MusicPlayer::ContainsMusic() 
	{
		return m_audioSync != 0;
	}

	bool MusicPlayer::Finished()
	{
		return false;
	}
	inline void MusicPlayer::loadAudio(Audio **audio, std::string audioName)
	{
		*audio = m_audioSystem->StreamSound(audioName + ".ogg", AudioType::AUDIO_TYPE_MUSIC);
		if (!*audio)
			*audio = m_audioSystem->StreamSound(audioName + ".mp3", AudioType::AUDIO_TYPE_MUSIC);
		if (!*audio)
			*audio = m_audioSystem->StreamSound(audioName + ".wav", AudioType::AUDIO_TYPE_MUSIC);

		if (*audio)
			m_audioSync = *audio;
	}
	inline void MusicPlayer::freeAudio(Audio **audio)
	{
		if (*audio)
		{
			m_audioSystem->DestroySound(*audio);
			*audio = 0;
		}
	}

	inline void MusicPlayer::playAudio(Audio *audio)
	{
		if (audio)
			audio->Play();
	}
	inline void MusicPlayer::pauseAudio(Audio *audio)
	{
		if (audio)
			audio->Pause();
	}
}

