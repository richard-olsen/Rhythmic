#include "song_start.hpp"

#include "../canvas.hpp"

#include "../../container/player_container.hpp"

#include "../../util/misc.hpp"

#include "../menu_system.hpp"
#include "../../game/stage/stage.hpp"

#include "../canvas_factory.hpp"

#include "../../util/thread_pool.hpp"

#include "../../game/engine_vals.hpp"

namespace Rhythmic
{
	WidgetSongStart::WidgetSongStart() :
		m_readyPlayers(0),
		m_threadFinished(false)
	{ }
	WidgetSongStart::~WidgetSongStart()
	{ }
	void WidgetSongStart::Update(float delta)
	{
		int currentPlayers = PlayerContainer::GetActivePlayers();
		if (m_readyPlayers == currentPlayers && currentPlayers > 0)
		{
			if (m_time < 0)
			{
				m_time = Util::GetTime();

				ThreadPool::QueueJob([this]() {
						PlayerContainer::EstablishActivePlayers(0);
						StageSystem::GetInstance()->SetPlaybackSpeed((float)GamePlayVariables::g_chartModifiers.m_songSpeed / 100.0f);
						m_threadFinished = true;
					});
			}

			if (Util::GetTime() - m_time >= 1.0 && m_threadFinished)
			{
				MenuSystem *menuSystem = MenuSystem::GetInstance();
				StageSystem *stageSystem = StageSystem::GetInstance();

				menuSystem->QueuePopMenu();
				menuSystem->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_IN_GAME_HUD));
				if (!stageSystem->GetModulePractice()->IsPracticeMode())
				{
					stageSystem->SetPause(false);
				}
			}

			//if (Util::GetTime() - m_time >= 1.0 && m_canvas->IsActive())
			//{
			//	/*MenuSystem *system = MenuSystem::GetInstance();

			//	system->QueuePopMenu();
			//	system->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_IN_GAME_HUD));

			//	StageSystem *stage = StageSystem::GetInstance();
			//	PlayerContainer::EstablishActivePlayers(0);
			//	stage->SetPause(false);*/

			//	// Set up some stage modules here such as on song end, and on song resume
			//}
		}
		else
		{
			m_time = -1;
		}
	}
	void WidgetSongStart::Render(float interpolation)
	{ }

	void WidgetSongStart::IncreasePlayersReady()
	{
		m_readyPlayers++;
	}
	void WidgetSongStart::DecreasePlayersReady()
	{
		m_readyPlayers--;
		if (m_readyPlayers < 0)
			m_readyPlayers = 0;
	}
}