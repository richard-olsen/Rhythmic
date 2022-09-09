#include "stage.hpp"

#include "../../events/event_system.hpp"
#include "../../chart/song.hpp"
#include "../../util/settings.hpp"

#include "../engine_vals.hpp"

#include "../../container/player_container.hpp"

#include <wge/core/logging.hpp>

#include "../../menus/menu_system.hpp"

#include "../../util/dear_imgui/imgui.h"

#include "practice_module.hpp"

#include <wge/core/time.hpp>

#include "../../util/thread_pool.hpp"

#include "../globals.hpp"
#include "../../container/bg_image_container.hpp"

#include "../../io/io_window.hpp"

#include <wge/io/filesystem.hpp>

namespace Rhythmic
{
	void StageSync::Init(MusicPlayer *player, float targetPlaybackSpeed)
	{
		m_targetPlaybackSpeed = targetPlaybackSpeed;
		m_targetPlaybackSpeedPercentage = 0;
		m_playbackSpeed = targetPlaybackSpeed;
		m_threshold = 0.005f;
		m_targetReverseTime = 0;
	}

	void StageSync::Sync(MusicPlayer *player)
	{
		// Set the playback speed with it's sped up / slowed down modification
		// without changing the target playback speed
		if (m_targetPlaybackSpeed < 0)
			m_playbackSpeed = m_targetPlaybackSpeed;
		else
			m_playbackSpeed = m_targetPlaybackSpeed + (m_targetPlaybackSpeedPercentage * m_targetPlaybackSpeed);

		// For animation purposes. An effect for reversing
		if (m_playbackSpeed < 0)
			m_songTime = glm::mix(m_songTime, m_targetReverseTime, 10.0f * WGE::Core::TimeStep.m_timeStep);
		else
			m_songTime += (double)(m_playbackSpeed * WGE::Core::TimeStep.m_timeStep);	// Use variable time step as it's capped and won't cause skipping

		if (m_playbackSpeed < 0)
			return;

		float pos = (float)player->GetDuration();		// Returns the playback head position
		
		float difference = m_songTime - pos;			// The difference to measure with the threshold

		// m_targetPlaybackSpeedPercentage should be interpolated, but not now. It's a proof of concept

		if (difference < -m_threshold) // Speed Up
			m_targetPlaybackSpeedPercentage = 0.1f;

		else if (difference > m_threshold) // Slow Down
			m_targetPlaybackSpeedPercentage = -0.1f;

		else							   // Keep playback speed normal
			m_targetPlaybackSpeedPercentage = 0;
	}

	StageSystem::StageSystem() :
		m_active(false),
		m_practiceModule(this),
		m_isReversing(false)
	{
	}

	void StageSystem::Init()
	{
		// DO NOT CREATE A LISTENER LIKE THIS
		// When this listener variable is gone, that's it. The function CAN'T be retrieved from the event system!
		//EventListener listener = EventAddListener(ET_IMGUI_TICK, CREATE_MEMBER_CB(StageSystem::TempImGUITick));
	}
	void StageSystem::Destroy()
	{

	}
	int speed = 100;
	void StageSystem::TempImGUITick(void *)
	{
		ImGui::Begin("Stage Options");

		ImGui::SliderInt("Song Speed", &speed, 10, 200, "%d\%");

		ImGui::Text(std::string("A Offset: " + std::to_string(GetModulePractice()->GetTimeStartOffsetted())).c_str());
		ImGui::Text(std::string("B Offset: " + std::to_string(GetModulePractice()->GetTimeEndOffsetted())).c_str());

		//if (!m_isReversing)
		//7	SetPlaybackSpeed((float)speed / 100.0f);

		ImGui::End();
	}
	void StageSystem::Update()
	{
		if (!m_active)
			return;

		bool containsMusic = m_songAudio.ContainsMusic();

		SettingValue value;
		value.type = SETTING_GAME_CALIBRATION_AUDIO;
		SettingsGet(&value);

		float time = m_stageSync.m_songTime - m_songChart.songData.offset - ((float)value.m_iValue * 0.001f);

		//m_playbackSpeed += delta * 0.1f;

		/*if (m_lastPlaybackSpeed != m_playbackSpeed)
		{
			m_lastPlaybackSpeed = m_playbackSpeed;
			m_songAudio.SetPlaybackSpeed(m_playbackSpeed);
		}*/

		// Sync
		if (!m_paused)
		{
			if (m_songAudio.GetDuration() == 0)// && (m_songTime + m_songAudio.GetPracticeOffset()) > 0) // Then we need to see if the songTime is within the range of the music
			{
				//double time = m_songTime + m_songAudio.GetPracticeOffset();

				if (m_stageSync.m_songTime > 0 && m_stageSync.m_songTime < m_songAudio.GetSyncLength()) // Then play audio
				{
					m_songAudio.SetDuration(m_stageSync.m_songTime);
					m_songAudio.Play();
				}
			}
			else
			{
				if (time > m_songAudio.GetSyncLength())
					m_songAudio.Pause();
			}


			//m_prevSongTime = m_songTime;
			//m_songTime += m_playbackSpeed * delta;

			//if (containsMusic && m_songTime < m_duration && m_songAudio.GetDuration() > 0) // Sometimes the song resets it's location back to 0 before the game calls ON_FINISHED event
			//{
			//	m_syncTime += delta;

			//	if (m_syncTime >= 1.0f)
			//	{
			//		m_syncTime = 0;


			//		float duration = m_songAudio.GetDuration() - m_songAudio.GetPracticeOffset();

			//		//LOG_INFO("Song Stats ({0}) ({1})", duration, m_songTime);

			//		m_songTime = (m_songTime + duration) * 0.5f; // Interpolate the values instead of setting it to the playback duration

			//		EventDispatch(ET_STAGE_ON_SYNC, 0);
			//	}
			//}
			m_stageSync.Sync(&m_songAudio);

			// When reversing, the current time of the song is about 1.5 seconds behind
			if (m_stageSync.m_songTime <= (m_songAudio.GetDuration() + 0.05f) && m_isReversing) // Stops the reversing effect
			{
				m_stageSync.m_targetPlaybackSpeed = m_lastPlaybackSpeed;
				m_songAudio.Play();
				m_isReversing = false;
			}

			m_chartTime = GetAdjustedPositionFromTime(time, m_songChart);

			// Update

			for (auto it = m_establishedPlayers.begin(); it != m_establishedPlayers.end(); it++)
				(*it)->Update(WGE::Core::TimeStep.m_fixedTimeStep, m_stageSync.m_songTime);

			EventDispatch(ET_STAGE_ON_UPDATE, 0);
		}

		// Stage finished
		if (time > m_duration && m_duration != -1)
		{
			EventDispatch(ET_STAGE_ON_FINISH); // Always send an OnStageFinish event whether practice mode is enabled or not
		}
		/*/else {
			if (m_songTime >= m_testTime + 0.5f) {
				m_testTime = m_songTime;

				LOG_INFO(m_songTime);
			}
		}/*/
	}

	PracticeModule *StageSystem::GetModulePractice()
	{
		return &m_practiceModule;
	}

	void StageSystem::Reset()
	{
		SettingValue value;
		value.type = SETTING_GAME_CALIBRATION_VIDEO;
		SettingsGet(&value);

		//std::vector<Player*> established = m_establishedPlayers;
		//m_establishedPlayers.clear();

		m_pauseTime = -20.0f;
		m_stageSync.m_targetReverseTime = -20.0f;

		m_noteCount = 0;

		float startTime = StageSystem::GetInstance()->GetModulePractice()->GetPositionStartOffsetted();
		float endTime = StageSystem::GetInstance()->GetModulePractice()->GetPositionEndOffsetted();

		/*value.type = SETTING_GAME_CALIBRATION_AUDIO;
		SettingsGet(&value);

		float nValue = value.m_iValue * 0.001f;

		startTime += nValue;
		endTime += nValue;*/

		for (int i = 0; i < m_establishedPlayers.size(); i++)
		{
			m_establishedPlayers[i]->GetInstrument()->Reset();

			int lastPosition = -1;
			Player* player = m_establishedPlayers[i];
			NotePool m_holdingPool = *player->GetInstrument()->GetNotePoolTemplate();

			for (size_t i = 0; i < m_holdingPool.size(); i++)
			{
				const Note& current = m_holdingPool[i];

				if (current.chartPos < startTime || current.chartPos >= endTime) // The current note must be within the time
					continue;

				if (current.chartPos != lastPosition)
				{
					lastPosition = current.chartPos;
					m_noteCount++;
				}
				// Drums can have more than one note on the same position
				else if (player->GetInstrument()->GetInstrumentType() == INSTRUMENT_TYPE_DRUMS)
				{
					lastPosition = current.chartPos;
					m_noteCount++;
				}
			}
		}
		m_songAudio.SetDuration(0);

		m_isReversing = false;

		m_stageSync.Init(&m_songAudio, m_songAudio.GetPlaybackSpeed());
		m_stageSync.m_songTime = -1.5 * m_stageSync.m_playbackSpeed;

		//m_songTime = -1.5 * m_playbackSpeed;
		SetPause(false);

		if (m_practiceModule.m_isPracticeMode)
			m_practiceModule.PrepareStage();
	}

	void StageSystem::Unload()
	{
		m_songAudio.Destroy();
		m_active = false;
		m_duration = -1;
		m_paused = true;
		
		m_songChart = Chart();
		beatlines.clear();

		m_isReversing = false;

		PlayerContainer::DisestablishActivePlayers();

		EventDispatch(ET_STAGE_ON_UNLOAD);
	}

	bool StageSystem::LoadInSong(const std::string &songPath)
	{
		m_syncTime = 0;
		m_stageSync.Init(&m_songAudio, 1.0f);
		m_stageSync.m_songTime = -1.5f;
		m_lastPlaybackSpeed = 1;
		m_chartTime = 0;
		m_pauseTime = -1;
		m_paused = true;
		m_isReversing = false;

		m_songAudio.Destroy();
		m_songAudio.Create(AudioSystem::GetInstance(), songPath);

		m_duration = m_songAudio.GetSyncLength();

		m_songChart = LoadChart(songPath);
		if (!m_songChart.supports[0] && !m_songChart.supports[1] && !m_songChart.supports[2])
			return false;

		GenBeatlines(m_songChart, &beatlines);
		StageCreateDataCB data;
		data.m_chart = &m_songChart;
		data.m_musicPlayer = &m_songAudio;
		data.m_system = this;

		m_practiceModule.SetTimeToWholeSong();
		m_active = true;

		SetPlaybackSpeed((float)Rhythmic::GamePlayVariables::g_chartModifiers.m_songSpeed / 100);

		SetBackgroundImage(BGContainer::GetBackground(Global_GetBGImage()));

		EventDispatch(ET_STAGE_ON_CREATE, &data);

		return true;
	}
	int StageSystem::LoadCalibration()
	{
		m_syncTime = 0;
		m_stageSync.Init(&m_songAudio, 1.0f);
		m_stageSync.m_songTime = -1.5f;
		m_lastPlaybackSpeed = 1;
		m_chartTime = 0;
		m_pauseTime = -1;
		m_paused = true;
		m_isReversing = false;

		m_songAudio.Destroy();
		m_songAudio.Create(AudioSystem::GetInstance(), WGE::FileSystem::GetFileInDirectory("game_data/misc/"), true);
		if (!m_songAudio.ContainsMusic())
			return 1;

		m_duration = m_songAudio.GetSyncLength();

		bool success = LoadChartFromChart(&m_songChart, WGE::FileSystem::GetFileInDirectory("game_data/misc/sync.chart"));
		if (!success)
		{
			m_songAudio.Destroy();
			return 2;
		}
		GenBeatlines(m_songChart, &beatlines);
		ChartSection empty;
		empty.position = 0;
		empty.time = 0;
		empty.endPosition = m_songChart.lastNotePosWithSustain;
		empty.endTime = GetAdjustedTimeFromPosition(empty.endPosition, m_songChart);
		m_songChart.listSection.push_back(empty);

		StageCreateDataCB data;
		data.m_chart = &m_songChart;
		data.m_musicPlayer = &m_songAudio;
		data.m_system = this;

		m_practiceModule.SetTimeToWholeSong();
		m_active = true;

		SetPlaybackSpeed(1.0f);

		SetBackgroundImage(BGContainer::GetBackground(Global_GetBGImage()));

		EventDispatch(ET_STAGE_ON_CREATE, &data);
		return 0;
	}
	void StageSystem::Reverse()
	{
		// Dont reverse in practice mode it's pretty annoying.
		if (m_isReversing || GetModulePractice()->IsPracticeMode())
			return;
		
		m_lastPlaybackSpeed = m_stageSync.m_targetPlaybackSpeed;

		m_pauseTime = m_stageSync.m_songTime;

		m_stageSync.m_targetPlaybackSpeed = -1.5f;

		double tryTarget = m_stageSync.m_songTime - 1.5f;
		if (tryTarget < m_stageSync.m_targetReverseTime) 
			tryTarget = m_stageSync.m_targetReverseTime;

		m_stageSync.m_targetReverseTime = tryTarget;

		m_songAudio.SetDuration(tryTarget);

		m_isReversing = true;
	}
	bool StageSystem::IsReversing()
	{
		return m_isReversing;
	}
	void StageSystem::SetPlaybackSpeed(float speed)
	{
		m_songAudio.SetPlaybackSpeed(speed);
		m_stageSync.m_targetPlaybackSpeed = speed;
	}
	void StageSystem::SetPause(bool pause)
	{
		m_paused = pause;
		if (pause)
		{
			m_pauseTime = m_stageSync.m_songTime;

			m_songAudio.Pause();
		}
		else if (!pause && (m_stageSync.m_songTime >= 0) && !m_isReversing)
		{
			m_songAudio.Play();
		}
	}
	void StageSystem::SetSongTime(float time, bool setMusicPlayersSongTime)
	{
		m_stageSync.m_songTime = time;
		if (setMusicPlayersSongTime)
			m_songAudio.SetDuration(time);
	}
	void StageSystem::SetDuration(float duration)
	{
		m_duration = duration;
	}

	MusicPlayer *StageSystem::GetMusicPlayer() { return &m_songAudio; }
	Chart *StageSystem::GetChart() { return &m_songChart; }
	int StageSystem::GetChartPosition() { return m_chartTime; }
	std::vector<Beatline> *StageSystem::GetBeatlines() { return &beatlines; }
	float StageSystem::GetPlaybackSpeed()
	{
		if (m_stageSync.m_targetPlaybackSpeed < 0)
			return m_lastPlaybackSpeed;
		return m_stageSync.m_targetPlaybackSpeed;
	}
	float StageSystem::GetSongTime() { return m_stageSync.m_songTime; }
	float StageSystem::GetDuration() { return m_duration; }
	float StageSystem::GetTimePaused() { return m_pauseTime; }

	int StageSystem::GetNoteCount()
	{
		return m_noteCount;
	}

	bool StageSystem::IsPlaying()
	{
		return !m_paused;
	}

	bool StageSystem::IsActive()
	{
		return m_active;
	}

	float StageSystem::GetPercentageComplete()
	{
		return m_stageSync.m_songTime / m_duration;
	}

	void StageSystem::EstablishPlayer(Player *player)
	{
		for (auto it = m_establishedPlayers.begin(); it != m_establishedPlayers.end(); it++)
		{
			if (*it == player)
				return;
		}

		m_establishedPlayers.push_back(player);
	}
	void StageSystem::DisestablishPlayer(Player *player)
	{
		for (auto it = m_establishedPlayers.begin(); it != m_establishedPlayers.end(); it++)
		{
			if (*it == player)
			{
				m_establishedPlayers.erase(it);
				return;
			}
		}
	}
	bool StageSystem::IsPlayerEstablished(Player *player)
	{
		for (auto it = m_establishedPlayers.begin(); it != m_establishedPlayers.end(); it++)
		{
			if (*it == player)
				return true;
		}
		return false;
	}
	std::vector<Player*> *StageSystem::GetEstablishedPlayers() { return &m_establishedPlayers; }

	void StageSystem::SetBackgroundImage(StageBackgroundIMG *image)
	{
		static unsigned char BLACK[4] = { 0,0,0,255 };

		if (!image)
			return;

		IO_Image io_image;
		io_image.data = 0;

		/*
		Only create the image if file name isn't empty

		"Black" - a default one has an empty file name!
		*/
		if (!image->m_fileName.empty())
			IO_LoadImage(WGE::FileSystem::GetFileInDirectory("custom/backgrounds/images/" + image->m_fileName), &io_image);

		if (io_image.data)
		{
			// Calculate Correct Size
			// glm::vec2(0.5f, 0.5f) will fit the entire screen regardless of window's aspect ratio
			glm::vec2 sizeWindow = glm::vec2(IO_Window_GetWidth(), IO_Window_GetHeight());
			glm::vec2 sizeImage = glm::vec2(io_image.width, io_image.height);

			
			float width = (sizeImage.x / sizeImage.y) * sizeWindow.y;
			float height = (sizeImage.y / sizeImage.x) * sizeWindow.x;
				
			bool widthLess = width < sizeWindow.x;
			bool heightLess = height < sizeWindow.y;
				
			if (widthLess)
			{
				width = sizeWindow.x;
				height = (sizeImage.y / sizeImage.x) * sizeWindow.x;
			}
			else if (heightLess)
			{
				height = sizeWindow.y;
				width = (sizeImage.x / sizeImage.y) * sizeWindow.y;
			}


			m_bgImageSize = (glm::vec2(width, height) / sizeWindow) * 0.5f;

			m_bgImage.Create(&io_image);

			IO_FreeImageData(&io_image);
		}
		else // Create black background
		{
			io_image.data = &BLACK[0];
			io_image.width = 1;
			io_image.height = 1;
			io_image.component = 4;
			m_bgImageSize = glm::vec2(0.5f);
			m_bgImage.Create(&io_image);
		}
	}
	Rendering::Texture *StageSystem::GetBackgroundImage()
	{
		return &m_bgImage;
	}
}