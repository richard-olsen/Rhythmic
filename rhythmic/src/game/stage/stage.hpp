#ifndef STAGE_SYSTEM_HPP_
#define STAGE_SYSTEM_HPP_

#include "../../events/event_system.hpp"
#include "../music_player.hpp"
#include "../../chart/chart.hpp"

#include "stage_renderer.hpp"

#include "../player.hpp"

#include "../../util/singleton.hpp"

#include "practice_module.hpp"

#include "../../container/bg_image.hpp"

namespace Rhythmic
{
	class StageSystem;

	class StageSync
	{
		friend class StageSystem;
	public:
		void Init(MusicPlayer *player, float targetPlaybackSpeed);

		void Sync(MusicPlayer *player);
	private:
		float m_playbackSpeed;						// The actual playback speed for the stage
		float m_targetPlaybackSpeed;				// The target playback speed (usually the same speed as Music Player's speed)
		float m_targetPlaybackSpeedPercentage;		// A percentage of the TPS added onto TPS like this: (TPS + (TPS * TPSPerc))

		double m_threshold;							// A +/- value for how far back or how far ahead SongTime can be

		double m_targetReverseTime;					// This is the target for how far back reversing will go. For the animation :)
		double m_songTime;
	};

	class StageSystem : public Singleton<StageSystem>
	{
		friend class Singleton<StageSystem>;
		friend class StageRenderer;
		friend class StageSync;
	public:
		void Init();
		void Destroy();

		void TempImGUITick(void *);

		void Update();

		// Get Modules
		PracticeModule *GetModulePractice();

		// Initializations
		bool LoadInSong(const std::string &songPath);
		int LoadCalibration();

		void Reset();

		/*
		Function reverses the scene about a second and a half, usually called after the player
		clicks on continue in the pause menu. Allows the players to back track a little and
		prepare for a continuation of the song

		This function invokes the reversing animation, and does nothing with syncing
		*/
		void Reverse();
		bool IsReversing();

		void SetPlaybackSpeed(float speed);
		void SetPause(bool pause);
		void SetSongTime(float time, bool setMusicPlayersSongTime = true);
		void SetDuration(float duration);

		void Unload();
		
		MusicPlayer *GetMusicPlayer();
		Chart *GetChart();
		int GetChartPosition();
		std::vector<Beatline> *GetBeatlines();
		float GetPlaybackSpeed();
		float GetSongTime();
		float GetDuration();
		float GetTimePaused();

		int GetNoteCount();

		bool IsPlaying();
		bool IsActive(); // Returns true if there is a song loaded in

		float GetPercentageComplete();

		void SetBackgroundImage(StageBackgroundIMG *image);
		Rendering::Texture *GetBackgroundImage();

		void EstablishPlayer(Player *player);
		void DisestablishPlayer(Player *player);
		bool IsPlayerEstablished(Player *player);
		std::vector<Player*> *GetEstablishedPlayers();
	private:
		StageSystem();

		Rendering::Texture m_bgImage;
		glm::vec2 m_bgImageSize;

		std::vector<Player*> m_establishedPlayers;

		StageSync			m_stageSync;					// Sync Module for the stage
		PracticeModule		m_practiceModule;				// Practice Module for the stage

		float m_syncTime;

		float m_lastPlaybackSpeed;					// The last playback speed. Just here to adjust speed when appropriate
		bool m_isReversing;

		double m_pauseTime;							// The time the stage was paused
		double m_duration;							// How long the song lasts for
		int m_chartTime;							// m_songTime in chart units
		bool m_paused;								// Should the stage continue moving the highways
		MusicPlayer m_songAudio;					// Collection of stem audio
		Chart m_songChart;							// The chart
		std::vector<Beatline> beatlines;		// The bpm measures

		bool m_active;								// Active stage or not

		int m_noteCount;							// Amount of notes
	};

	struct StageCreateDataCB // Structure for the stage create callback
	{
		StageSystem *m_system;
		MusicPlayer *m_musicPlayer;
		Chart *m_chart;
	};
}

#endif

