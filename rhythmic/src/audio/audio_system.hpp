#ifndef AUDIO_SYSTEM_H_
#define AUDIO_SYSTEM_H_

#include <string>
#include <vector>

#include "../util/singleton.hpp"

#include "audio.hpp"

namespace Rhythmic
{
	class AudioSystem : public Singleton<AudioSystem>
	{
		friend class Singleton<AudioSystem>;
	public:
		AudioSystem();
		~AudioSystem();

		bool Initialize();
		void Destroy();

		Audio *CreateSound(const std::string &file, AudioType type = AUDIO_TYPE_SFX);
		Audio *StreamSound(const std::string &file, AudioType type = AUDIO_TYPE_MUSIC);
		void DestroySound(Audio *sound);

		void						*GetLowLevelSystem();

		void SetMasterVolume(float volume);
		void SetVolume(AudioType type, float volume);

		void Update();
	private:
		std::vector<Audio*> m_audio;
	};
}

#endif

