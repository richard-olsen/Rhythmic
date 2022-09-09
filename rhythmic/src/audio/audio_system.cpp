#include "audio_system.hpp"

#include <iostream>

#include <wge/core/logging.hpp>

#include "../util/misc.hpp"
#include "../util/settings.hpp"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <bass.h>

namespace Rhythmic
{
	extern GLFWwindow *g_window;

	AudioSystem::AudioSystem() :
		m_audio()
	{
	}

	AudioSystem::~AudioSystem()
	{ }

	bool AudioSystem::Initialize()
	{
		if (HIWORD(BASS_GetVersion()) != BASSVERSION)
		{
			LOG_CRITICAL("Incompatible version of BASS.DLL");
			return false;
		}

		if (!BASS_Init(-1, 44100, 0, glfwGetWin32Window(g_window), NULL))
		{
			return false;
		}

		return true;
	}
	void AudioSystem::Destroy()
	{
		BASS_Free();
	}

	Audio *AudioSystem::CreateSound(const std::string &file, AudioType type)
	{
		return 0;
	}
	Audio *AudioSystem::StreamSound(const std::string &file, AudioType type)
	{
		HSTREAM stream = BASS_StreamCreateFile(FALSE, file.c_str(), 0, 0, 0);
		
		if (stream == 0)
			return 0;

		Audio *audio = new Audio(this, stream, 1.0f, 1.0f, type);
		m_audio.push_back(audio);
		return audio;
	}
	void AudioSystem::DestroySound(Audio *sound)
	{
		if (!sound)
			return;

		BASS_StreamFree(sound->getStream());
		auto it = std::find(m_audio.begin(), m_audio.end(), sound);
		if (it != m_audio.end())
		{
			m_audio.end();
		}
		delete sound;
	}

	void *AudioSystem::GetLowLevelSystem()
	{
		return 0;
	}

	void AudioSystem::SetMasterVolume(float volume)
	{
	}
	void AudioSystem::SetVolume(AudioType type, float volume)
	{
	}

	void AudioSystem::Update()
	{
		
	}
}
