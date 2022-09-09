#include "audio.hpp"

#include "audio_system.hpp"

#include <string>

#include <wge/core/logging.hpp>

#include <bass.h>

namespace Rhythmic
{
	Audio::Audio(AudioSystem *system, int stream, float masterVolume, float audioVolume, AudioType type) :
		system(system),
		stream(stream),
		type(type),
		channel(0)
	{
	}
	Audio::~Audio()
	{
	}

	int Audio::getStream()
	{
		return stream;
	}

	void Audio::Play()
	{
		BASS_ChannelPlay(stream, FALSE);
	}
	void Audio::Pause()
	{
		BASS_ChannelPause(stream);
	}
	void Audio::Reset()
	{
		BASS_ChannelSetPosition(stream, 0, BASS_POS_BYTE);
	}

	bool Audio::IsPlaying()
	{
		return false;
	}

	void Audio::SetPlaybackSpeed(float speed)
	{
	}

	void Audio::SetPlaybackDuration(double duration)
	{
	}


	double Audio::GetPlaybackDuration()
	{
		QWORD byteLength = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
		return BASS_ChannelBytes2Seconds(stream, byteLength);
	}
	double Audio::GetLengthInSeconds()
	{
		QWORD byteLength = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		return BASS_ChannelBytes2Seconds(stream, byteLength);
	}

	void Audio::AddDSP(void *dsp, int index) {
	}

	void Audio::RemoveDSP(void *dsp) {
	}

	void Audio::GetDSP(void **dsp, int index) {
	}

	void Audio::SetLocalVolume(float volume)
	{
	}

	void *Audio::GetChannel()
	{
		return channel;
	}

	AudioType Audio::GetAudioType()
	{
		return type;
	}
}