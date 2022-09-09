#ifndef CANVAS_FACTORY_SONG_CURRENT_HPP_
#define CANVAS_FACTORY_SONG_CURRENT_HPP_

#include "../../engine/include/wge/core/logging.hpp"

#include "../../container/engine/engine_container.hpp"

#include "../canvas.hpp"

#include "../widget.hpp"

#include "../widgets/game_pause.hpp"
#include "../widgets/text.hpp"
#include "../widgets/progress_bar.hpp"

#include "../../events/event_system.hpp"
#include "../../events/events_common.hpp"
#include "../menu_system.hpp"
#include "../../rendering/render_manager.hpp"

namespace Rhythmic
{
	class WidgetOnSongFinished : public WidgetBase
	{
	public:
		WidgetOnSongFinished()
		{
			StageSystem *stageSystem = StageSystem::GetInstance();
			if (stageSystem->GetModulePractice()->IsPracticeMode())
			{
				m_listener = EventAddListener(ET_STAGE_ON_FINISH, [stageSystem](void*)
					{
						//stageSystem->SetSongTime(-1.5 * stageSystem->GetPlaybackSpeed());
						stageSystem->Reset();
						stageSystem->SetPause(false);
					});
			}
			else
			{
				m_listener = EventAddListener(ET_STAGE_ON_FINISH, [stageSystem](void*)
					{
						MenuSystem *menuSystem = MenuSystem::GetInstance();
						menuSystem->QueuePopMenu();
						menuSystem->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SONG_STATS));
						stageSystem->Unload();
					});
			}
		}
		~WidgetOnSongFinished()
		{
			EventRemoveListener(m_listener);
		}

		virtual void Update(float delta) { }
		virtual void Render(float interpolation) { }
	private:
		EventListener m_listener;
	};

	class WidgetScore : public WidgetText
	{
	public:
		WidgetScore() :
			WidgetText("", RenderManager::GetFontRef("menus"), 0)
		{ }
		virtual ~WidgetScore()
		{ }

		virtual void Update(float delta)
		{
			std::vector<Player*> *players = StageSystem::GetInstance()->GetEstablishedPlayers();

			int score = 0;
			for (int i = 0; i < players->size(); i++)
				score += (*players)[i]->GetInstrument()->GetScore();

			SetText(std::to_string(score));
		}
	};
	class WidgetSectionDisplay : public WidgetText
	{
	public:
		WidgetSectionDisplay() :
			WidgetText("", RenderManager::GetFontRef("menus"), 1)
		{ }
		virtual ~WidgetSectionDisplay() {}

		virtual void Update(float delta)
		{
			StageSystem *stageSystem = StageSystem::GetInstance();

			Chart *chart = stageSystem->GetChart();

			if (chart->listSection.empty())
				return;

			float time = stageSystem->GetSongTime();

			if (time < chart->listSection[0].time)
			{
				if (GetText() != chart->listSection[0].name)
					SetText(chart->listSection[0].name);
				return;
			}

			for (size_t i = 0; i < chart->listSection.size(); i++)
			{
				ChartSection &section = chart->listSection[i];
				if (time >= section.time && time < section.endTime)
				{
					if (GetText() != section.name)
						SetText(section.name);
					break;
				}
			}
		}
	private:
	};
	class WidgetNoteCount : public WidgetText
	{
	public:
		WidgetNoteCount() :
			WidgetText("", RenderManager::GetFontRef("menus"), 1)
		{
		}
		virtual ~WidgetNoteCount()
		{
		}

		virtual void Update(float delta)
		{
			std::vector<Player*>* players = StageSystem::GetInstance()->GetEstablishedPlayers();

			int notesHit = 0;
			for (int i = 0; i < players->size(); i++)
				notesHit += (*players)[i]->GetInstrument()->GetNotesHit();

			int noteCount = StageSystem::GetInstance()->GetNoteCount();

			float division = 1.0f;
			if (noteCount != 0) division = noteCount;

			int percentage = floor(((float)notesHit / division) * 100);

			SetText(std::to_string(notesHit) + "/" + std::to_string(noteCount) + " - " + std::to_string(percentage) + "%");
		}
	private:
		int lastHitCount = -1;
		std::vector<Player*>* players = StageSystem::GetInstance()->GetEstablishedPlayers();
	};

	class WidgetNoteStreak : public WidgetText
	{
	public:
		WidgetNoteStreak() :
			WidgetText("", RenderManager::GetFontRef("menus"), 2)
		{ }
		virtual ~WidgetNoteStreak()
		{ }

		virtual void Update(float delta)
		{
			std::vector<Player*> *players = StageSystem::GetInstance()->GetEstablishedPlayers();

			int noteStreak = 0;
			for (int i = 0; i < players->size(); i++)
				noteStreak += (*players)[i]->GetInstrument()->GetStreak();

			SetText(std::to_string(noteStreak));
		}
	};

	class WidgetMissedNotes : public WidgetText
	{
	public:
		WidgetMissedNotes() :
			WidgetText("", RenderManager::GetFontRef("menus"), 1)
		{ }
		virtual ~WidgetMissedNotes()
		{ }

		virtual void Update(float delta)
		{
			std::vector<Player*>* players = StageSystem::GetInstance()->GetEstablishedPlayers();

			int missedNotes = 0;
			for (int i = 0; i < players->size(); i++)
				missedNotes -= (*players)[i]->GetInstrument()->GetNotesMissed();
				

			SetText(std::to_string(missedNotes));
		}
	};

	class WidgetPracticeSpeed : public WidgetText
	{
	public:
		WidgetPracticeSpeed() :
			WidgetText("", RenderManager::GetFontRef("menus"), 1)
		{ }
		virtual ~WidgetPracticeSpeed()
		{ }

		virtual void Update(float delta)
		{
			SetText("< " + std::to_string((int)(round(StageSystem::GetInstance()->GetPlaybackSpeed() * 100))) + "% >");
		}
	};

	class WidgetSongProgress : public WidgetProgressBar
	{
	public:
		WidgetSongProgress() :
			WidgetProgressBar(0)
		{
		}
		virtual ~WidgetSongProgress()
		{ }

		virtual void Update(float delta)
		{
			float progress = 0;
			StageSystem *stageSystem = StageSystem::GetInstance();

			PracticeModule *practiceModule = stageSystem->GetModulePractice();

			// Practice mode has a different way of calculating the song progress
			// as it is going by the length of the selected section(s)
			if (practiceModule->IsPracticeMode())
			{
				float time = stageSystem->GetSongTime();
				progress = (time - practiceModule->GetTimeStartOffsetted()) / (practiceModule->GetTimeEndOffsetted() - practiceModule->GetTimeStartOffsetted());
			}
			else
			{
				progress = stageSystem->GetPercentageComplete();
			}
			progress = progress <= 0 ? 0 : (progress >= 1 ? 1 : progress); // Clamp the progress between 0 - 1
			SetProgress(progress);
			WidgetProgressBar::Update(delta);
		}
	};

	class WidgetEngineDisplay : public WidgetText
	{
	public:
		WidgetEngineDisplay() :
			WidgetText("Engine: " + EngineContainer::GetCurrentEngine()->m_name + (EngineContainer::GetCurrentEngine()->m_isCustom ? " (Custom)" : ""), RenderManager::GetFontRef("menus"), 1)
		{
		}
		virtual ~WidgetEngineDisplay()
		{
		}

	};

	/*
	This class controls the state of the Player Display widget
	and only allows it to function when the game is paused
	*/
	class WidgetPlayerDisplayController : public WidgetBase
	{
	public:
		WidgetPlayerDisplayController() :
			m_display(),
			m_stage(StageSystem::GetInstance())
		{
			SetCanvasChangeCallback([this](Canvas *canvas) {m_display.SetCanvas(canvas);});
		}
		~WidgetPlayerDisplayController()
		{}

		void Update(float delta) override
		{
			if (!m_stage->IsPlaying())
				m_display.Update(delta);
		}
		void Render(float interpolation) override
		{
			if (!m_stage->IsPlaying())
				m_display.Render(interpolation);
		}
		void ProcessInput(InputEventData *input, int playerIndex) override
		{
			if (!m_stage->IsPlaying())
				m_display.ProcessInput(input, playerIndex);
		}
	private:
		WidgetPlayerDisplay m_display;
		StageSystem *m_stage;
	};

	namespace FactoryCanvas
	{
		void CanvasCreateSongCurrent(Canvas *canvas, void *canvasData)
		{
			// Add stuff here, any widget added will be shown while the song is playing and players are getting their fc's

			canvas->Add("p0", new WidgetOnSongFinished());

			canvas->Add("ffs", new WidgetPlayerDisplayController());

			WidgetButtonField *buttonField = new WidgetButtonField(glm::vec4(0,0,0,0.6f));
			canvas->Add("p1", buttonField);
			buttonField->SetPosition(glm::vec2(0.7f, 0.4f));
			buttonField->SetScale(glm::vec2(0.2f, 0.6f));

			WidgetPauseFunc *pausingWidget = new WidgetPauseFunc(buttonField);
			canvas->Add("p3", pausingWidget);

			WidgetScore *scorePreview = new WidgetScore();
			scorePreview->SetPosition(glm::vec2(0.3f, 0.1f));
			scorePreview->SetScale(glm::vec2(0.02f));
			canvas->Add("p6", scorePreview);

			WidgetNoteStreak *streakPreview = new WidgetNoteStreak();
			streakPreview->SetPosition(glm::vec2(0.7f, 0.1f));
			streakPreview->SetScale(glm::vec2(0.02f));
			canvas->Add("p7", streakPreview);

			WidgetSongProgress *songProgress = new WidgetSongProgress();
			songProgress->SetExtents(glm::vec2(0.3f, 0.15f), glm::vec2(0.7f, 0.17f));
			canvas->Add("p8", songProgress);

			if (StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode())
			{
				WidgetEngineDisplay* engine = new WidgetEngineDisplay();
				engine->SetPosition(songProgress->GetPosition() - glm::vec2(0, 0.12f));
				engine->SetScale(glm::vec2(0.015f));
				canvas->Add("engine", engine);

				WidgetSectionDisplay *display = new WidgetSectionDisplay();
				display->SetPosition(songProgress->GetPosition() + glm::vec2(0, 0.065f));
				display->SetScale(glm::vec2(0.02f));
				canvas->Add("d", display);
				WidgetPracticeToSectionSelection *pssoe = new WidgetPracticeToSectionSelection();
				pausingWidget->SetPracticeRedirect(pssoe);
				canvas->Add("pssoe", pssoe);

				WidgetNoteCount *count = new WidgetNoteCount();
				count->SetPosition(glm::vec2(0.5f, 0.1f));
				count->SetScale(glm::vec2(0.02f));
				canvas->Add("count", count);

				WidgetPracticeSpeed* speed = new WidgetPracticeSpeed();
				speed->SetPosition(glm::vec2(0.1f, 0.1f));
				speed->SetScale(glm::vec2(0.02f));
				canvas->Add("speed", speed);
			}
			else
			{
				SettingValue value;
				value.type = SETTING_GAME_SHOW_MISSED_NOTES;
				SettingsGet(&value);

				if (value.m_iValue != 0)
				{
					WidgetMissedNotes* missed = new WidgetMissedNotes();
					missed->SetPosition(glm::vec2(0.1f, 0.1f));
					missed->SetScale(glm::vec2(0.02f));
					canvas->Add("missed", missed);
				}
			}
		}
	}
}

#endif

