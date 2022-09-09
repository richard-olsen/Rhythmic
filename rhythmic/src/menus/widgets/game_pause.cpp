#include "game_pause.hpp"

#include "../../game/stage/stage.hpp"
#include "../menu_system.hpp"

#include "../../util/thread_pool.hpp"

#include "../canvas_factory.hpp"

#include "../../container/player_container.hpp"

#include "../../engine/include/wge/core/logging.hpp"

namespace Rhythmic
{
	WidgetPracticeToSectionSelection::WidgetPracticeToSectionSelection() :
		m_didRedirect(false)
	{

	}
	void WidgetPracticeToSectionSelection::Update(float delta)
	{
		if (!m_didRedirect)
		{
			MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SECTION_SELECTION));
			m_didRedirect = true;
		}
	}
	void WidgetPracticeToSectionSelection::Render(float interpolate) {}


	WidgetPauseFunc::WidgetPauseFunc(WidgetButtonField *pauseButtons) :
		m_pauseCanvasButtonField(pauseButtons),
		m_pausedPlayer(0),
		m_wantSectionChange(false),
		m_toSectionRedirect(0),
		m_allowPausing(false)
	{

	}
	WidgetPauseFunc::~WidgetPauseFunc()
	{ }
	void WidgetPauseFunc::Update(float delta)
	{
		StageSystem *stage = StageSystem::GetInstance();

		if (!stage->IsActive())
			return;

		if (stage->IsPlaying())
		{
			m_allowPausing = true;

			std::vector<Player*> *activePlayers = stage->GetEstablishedPlayers();
			// Don't want players not established to be able to pause the game

			if (m_pausedPlayer != 0)
			{
				m_pauseCanvasButtonField->ClearButtons();
				m_pausedPlayer = 0;
			}

			for (int i = 0; i < activePlayers->size(); i++)
			{
				Player *player = (*activePlayers)[i];

				//if (player->GetInstrument()->GetInputDevice()->hitDataButton[Input::BUTTON_START].pressed)
				//{
				//	stage->SetPause(true);
				//	m_pausedPlayer = player;
				//	EmplaceButtons();
				//}
			}
		}
		else
		{
			if (m_toSectionRedirect != 0)
			{
				if (!m_toSectionRedirect->m_didRedirect)
					return;

				if (m_toSectionRedirect->m_didRedirect && !m_wantSectionChange && !m_allowPausing)
				{
					StageSystem::GetInstance()->Unload();
					MenuSystem::GetInstance()->QueuePopMenu();
					return;
				}
			}

			if (m_pausedPlayer == 0)
			{
				// Must find the correct player
				m_pausedPlayer = (*stage->GetEstablishedPlayers())[0];
				EmplaceButtons();
			}
		}
	}
	void WidgetPauseFunc::Render(float interpolation)
	{ }
	void WidgetPauseFunc::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex < 0 || playerIndex > 3)
			return;

		StageSystem *system = StageSystem::GetInstance();

		if (input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_START) && system->IsPlaying())
		{
			system->SetPause(true);
			m_pausedPlayer = PlayerContainer::GetPlayerByIndex(playerIndex)->m_player;
			EmplaceButtons();
		}
	}

	void WidgetPauseFunc::SetPracticeRedirect(WidgetPracticeToSectionSelection *selectionRedirect)
	{
		m_toSectionRedirect = selectionRedirect;
	}

	void WidgetPauseFunc::EmplaceButtons()
	{
		m_pauseCanvasButtonField->AddButton(new WidgetButton("Resume", []()
			{
				StageSystem *system = StageSystem::GetInstance();
				system->Reverse();
				system->SetPause(false);
				
				std::vector<Player*> *players = system->GetEstablishedPlayers();

				for (auto player = players->begin(); player != players->end(); player++)
				{
					auto instrument = (*player)->GetInstrument();
					if (!instrument->IsBotControlling())
						instrument->CorrectInputValues();
				}

				return BUTTON_FINISHED;
			}));
		m_pauseCanvasButtonField->AddButton(new WidgetButton("Restart", []()
			{
				if (!StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode())
				{
					MenuSystem::GetInstance()->DisplayConfirmationBox([]()
						{
							StageSystem::GetInstance()->Reset();
							StageSystem::GetInstance()->SetPause(false);
						}, "Are you sure you want to restart?");
				}
				else
				{
					StageSystem::GetInstance()->Reset();
					StageSystem::GetInstance()->SetPause(false);
				}
				return BUTTON_FINISHED;
			}));
		if (StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode())
		{
			m_pauseCanvasButtonField->AddButton(new WidgetButton("Set A Position", []()
				{
					int position = StageSystem::GetInstance()->GetChartPosition();

					//LOG_INFO(position);

					StageSystem::GetInstance()->GetModulePractice()->SetTimeAOffset(position);
					return BUTTON_FINISHED;
				}));
			m_pauseCanvasButtonField->AddButton(new WidgetButton("Set B Position", []()
				{
					int position = StageSystem::GetInstance()->GetChartPosition();

					//LOG_INFO(position);

					StageSystem::GetInstance()->GetModulePractice()->SetTimeBOffset(position);

					return BUTTON_FINISHED;
				}));
			m_pauseCanvasButtonField->AddButton(new WidgetButton("Clear A/B", []()
				{
					StageSystem::GetInstance()->GetModulePractice()->ResetAB();
					return BUTTON_FINISHED;
				}));
			m_pauseCanvasButtonField->AddButton(new WidgetButton("Change Section", []()
				{
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_SECTION_SELECTION));
					return BUTTON_FINISHED;
				}));
		}
		m_pauseCanvasButtonField->AddButton(new WidgetButton("Options", []()
			{
				MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_PAUSE_OPTIONS));
				return BUTTON_FINISHED;
			}));
		m_pauseCanvasButtonField->AddButton(new WidgetButton("Quit", []()
			{
				MenuSystem::GetInstance()->DisplayConfirmationBox([]()
					{
						StageSystem::GetInstance()->Unload();
						MenuSystem::GetInstance()->QueuePopMenu();
					}, "Are you sure you want to quit?");
				return BUTTON_FINISHED;
			}));
		m_pauseCanvasButtonField->ForceSelect(0);
	}
}

