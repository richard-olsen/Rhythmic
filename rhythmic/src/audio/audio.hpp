#ifndef AUDIO_H_
#define AUDIO_H_

namespace Rhythmic
{
	enum AudioType
	{
		AUDIO_TYPE_MUSIC,
		AUDIO_TYPE_SFX,
		AUDIO_TYPE_size
	};

	class AudioSystem;
	class Audio
	{
	public:
		Audio(AudioSystem *system, int sound, float masterVolume, float audioVolume, AudioType type);
		~Audio();

		int getStream();

		void Play();
		void Pause();
		void Reset();

		bool IsPlaying();

		void SetPlaybackSpeed(float speed);

		void SetPlaybackDuration(double duration);
		double GetPlaybackDuration();

		double GetLengthInSeconds();

		void AddDSP(void *dsp, int index);
		void RemoveDSP(void *dsp);
		void GetDSP(void **dsp, int index);

		void SetLocalVolume(float volume);

		void *GetChannel();

		AudioType GetAudioType();
	private:
		AudioSystem *system;
		AudioType type;

		int stream;
		void *channel;
		float frequency;
	};
} // namespace Rhythmic

#endif

