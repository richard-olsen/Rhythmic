#include "../canvas.hpp"
#include "../widget.hpp"
#include "../menu_system.hpp"
#include "../../events/event_system.hpp"

#include "../../game/instrument/instrument_calibrate.hpp"
#include "../../game/menu_music_player.hpp"

#include <functional>

namespace Rhythmic
{
	class WidgetCalibrate : public WidgetBase
	{
	public:
		WidgetCalibrate() :
			WidgetBase(),
			m_stateChange(false),
			m_isPlayerBot(false),
			m_onStageEnd(ET_STAGE_ON_FINISH, CREATE_MEMBER_CB(WidgetCalibrate::OnStageEnd)),
			m_state(0)
		{ }
		~WidgetCalibrate()
		{ }

		void OnStageEnd(void *)
		{
			InstrumentCalibrate *instrument = (InstrumentCalibrate*)PlayerContainer::GetPlayerByIndex(0)->m_player->GetInstrument();
			std::vector<float> &times = instrument->GetTimesNoteHit();

			bool shouldRedo = times.size() != 12;

			if (!shouldRedo)
			{
				int value = -Calibrate(instrument->GetNotes());
				

				if (m_state == 1) // Video
				{
					m_calibratedVideo = value;
				}
				else if (m_state == 3) // Audio
				{
					m_calibratedAudio = value;
				}

				m_state++;
			}
			
			StageSystem::GetInstance()->Unload();
			m_stateChange = true;
		}

		int Calibrate(NotePool *pool)
		{
			int calibration = 0;

			for (int i = 0; i < 12; i++)
			{
				Note &note = (*pool)[i];
				calibration += (int)((note.time - note.lastScoreCheck) * 1000);
			}
			
			calibration /= 12;

			return calibration;
		}

		void Update(float delta) override
		{
			if (m_stateChange)
			{
				m_stateChange = false;
				ChangeState();
			}
		}

		void Render(float interpolation) override
		{

		}

		void ChangeState()
		{
			if (m_state == 0)
			{
				MusicPlayerMenu::GetInstance()->Stop();
				MenuSystem::GetInstance()->DisplayMessage("Let's start with video! Strum the incoming twelve notes");
				m_state++;
			}
			else if (m_state == 1)
			{
				SetUp(0);
			}
			else if (m_state == 2)
			{
				MenuSystem::GetInstance()->DisplayMessage("Now let's do audio! Strum when you hear the sound");
				m_state++;
			}
			else if (m_state == 3)
			{
				SetUp(1);
			}
			else
			{
				SettingValue value;
				
				value.type = SETTING_GAME_CALIBRATION_VIDEO;
				value.m_iValue = m_calibratedVideo;
				SettingsSet(value);

				value.type = SETTING_GAME_CALIBRATION_AUDIO;
				value.m_iValue = m_calibratedAudio;
				SettingsSet(value);

				MenuSystem::GetInstance()->QueuePopMenu();
				MusicPlayerMenu::GetInstance()->Start();
			}
		}

		void OnCanvasActivated() override
		{
			ChangeState();
			/*if (!m_initialized)
			{
				SetUp();
			}*/
		}

		void OnCanvasDeactivated() override
		{
		}

		void SetUp(float volume)
		{
			MenuSystem *system = MenuSystem::GetInstance();

			int state = StageSystem::GetInstance()->LoadCalibration();

			if (state == 1)
			{
				system->QueuePopMenu();
				system->DisplayMessage("Sync Sound Not Found!");
				return;
			}
			if (state == 2)
			{
				system->QueuePopMenu();
				system->DisplayMessage("Sync Chart Invalid / Not Found!");
				return;
			}


			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(0);
			/*if (player->m_settings.useBot)
				m_isPlayerBot = player->m_settings.useBot;*/
			//player->m_settings.useBot = false;
			player->m_instrument = -1;
			player->m_difficulty = NOTES_EXPERT;
			player->m_player->EstablishPlayer();

			StageSystem::GetInstance()->GetMusicPlayer()->SetLocalVolume(volume);
			StageSystem::GetInstance()->SetPause(false);
		}
	private:
		bool m_stateChange;

		bool m_isPlayerBot;

		EventListener_raii m_onStageEnd;

		/*
		0 - Show message
		1 - Generate stage
		2 - Show message
		3 - Genearte stage
		4 - Go back to options
		*/
		int m_state;

		int m_calibratedVideo;
		int m_calibratedAudio;
	};

	namespace FactoryCanvas
	{
		void CanvasCreateCalibration(Canvas *canvas, void *canvasData)
		{
			WidgetCalibrate *calibrate = new WidgetCalibrate();

			canvas->Add("p0", calibrate);
		}
	}
}