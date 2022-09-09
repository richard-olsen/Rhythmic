#ifndef CANVAS_FACTORY_SONG_PREP_HPP_
#define CANVAS_FACTORY_SONG_PREP_HPP_

#include "../canvas.hpp"

#include "../widgets/canvas_preview.hpp"
#include "../widgets/player_prep.hpp"
#include "../widgets/song_start.hpp"
#include "../widgets/controller_back_menu.hpp"

#include "../widget_util.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../util/misc.hpp"

#include "../../container/song_container.hpp"
#include "../../container/player_container.hpp"

#include "../../io/io_window.hpp"

#include "../menu_system.hpp"

#include "../../game/stage/stage.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		void CanvasCreateSongPrep(Canvas *canvas, void *canvasData)
		{
			canvas->Add("bg", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas()));

			Chart *chart = StageSystem::GetInstance()->GetChart(); // Need to get the chart

			WidgetTile *tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			canvas->Add("p4", tile);

			WidgetSongStart *song_start = new WidgetSongStart();
			canvas->Add("p9", song_start);

			canvas->Add("p0", new WidgetPlayerPrep(chart, 0, song_start)); // Player 0 Sel
			canvas->Add("p1", new WidgetPlayerPrep(chart, 1, song_start)); // Player 1 Sel
			canvas->Add("p2", new WidgetPlayerPrep(chart, 2, song_start)); // Player 2 Sel
			canvas->Add("p3", new WidgetPlayerPrep(chart, 3, song_start)); // Player 3 Sel

			std::string speed = " (" + std::to_string((int)(StageSystem::GetInstance()->GetPlaybackSpeed() * 100)) + "%)";

			WidgetText *songNameText = new WidgetText(chart->songData.name +
				(StageSystem::GetInstance()->GetPlaybackSpeed() != 1.0f ? speed : ""), RenderManager::GetFontRef("menus"), 1);
			songNameText->SetPosition(glm::vec2(0.5f, 0.1f));
			songNameText->SetScale(glm::vec2(0.03f, 0.03f));

			WidgetText *songArtistText = new WidgetText(chart->songData.artist, RenderManager::GetFontRef("menus"), 1);
			songArtistText->SetPosition(glm::vec2(0.5f, 0.15f));
			songArtistText->SetScale(glm::vec2(0.02f, 0.02f));

			canvas->Add("p5", songNameText);
			canvas->Add("p6", songArtistText);
			if (!chart->songData.charter.empty())
			{
				WidgetText *songCharterText = new WidgetText("Charted By: " + chart->songData.charter, RenderManager::GetFontRef("menus"), 1);
				songCharterText->SetPosition(glm::vec2(0.5f, 0.9f));
				songCharterText->SetScale(glm::vec2(0.02f, 0.02f));
				canvas->Add("p7", songCharterText);
			}

			WidgetButton *buttonBack = new WidgetButton("<", []() { StageSystem::GetInstance()->Unload(); MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);

			canvas->Add("p8", new WidgetPlayerDisplay());
		}
	}
}


#endif

