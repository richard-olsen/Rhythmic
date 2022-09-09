#ifndef MUSIC_PLAYER_H_
#define MUSIC_PLAYER_H_

#include "../audio/audio.hpp"
#include "../audio/audio_system.hpp"

namespace Rhythmic
{
	class MusicPlayer
	{
	public:
		MusicPlayer();
		~MusicPlayer();

		void Create(AudioSystem *system, const std::string &songPath, bool isSync = false);
		void Destroy();

		void Play();
		void Pause();

		double GetDuration();
		double GetSyncLength();
		float GetPlaybackSpeed();

		void SetDuration(double duration);
		void SetLocalVolume(float volume);
		void SetPlaybackSpeed(float speed);

		bool IsPlaying();

		bool ContainsMusic();

		bool Finished();
	private:
		inline void loadAudio(Audio **audio, std::string audioName);
		inline void freeAudio(Audio **audio);
		inline void playAudio(Audio *audio);
		inline void pauseAudio(Audio *audio);

		float			m_playbackSpeed;

		AudioSystem		*m_audioSystem;

		void			*m_channelGroup;
		void			*m_pitchDSP;

		Audio			*m_audioSync;

		Audio			*m_audioGuitar;
		Audio			*m_audioRhythm;
		Audio			*m_audioDrums;
		Audio			*m_audioDrums1;
		Audio			*m_audioDrums2;
		Audio			*m_audioDrums3;
		Audio			*m_audioDrums4;
		Audio			*m_audioKeys;
		Audio			*m_audioSong;
		Audio			*m_audioLyrics;
	};
}

#endif

