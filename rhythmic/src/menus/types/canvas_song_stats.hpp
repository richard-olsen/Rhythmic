#ifndef CANVAS_FACTORY_SONG_STATS_HPP_
#define CANVAS_FACTORY_SONG_STATS_HPP_

#include "../canvas.hpp"

#include "../widgets/canvas_preview.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"

#include "../menu_system.hpp"

#include "../../rendering/render_manager.hpp"

#include "../widgets/player_stats.hpp"

#include <string>

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		void CanvasCreateSongStats(Canvas *canvas, void *canvasData)
		{
			canvas->Add("p0", new WidgetCanvasPreview(MenuSystem::GetInstance()->QueryCanvas(1)));

			WidgetTile *tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			canvas->Add("p4", tile);

			for (int i = 0; i < 4; i++)
				canvas->Add(("pS" + std::to_string(6 + i)).c_str(), new WidgetPlayerStats(i));
			
			Chart *chart = StageSystem::GetInstance()->GetChart();

			std::string speed = " (" + std::to_string((int)(StageSystem::GetInstance()->GetPlaybackSpeed() * 100)) + "%)";

			WidgetText *songNameText = new WidgetText(chart->songData.name + 
				(StageSystem::GetInstance()->GetPlaybackSpeed() != 1.0f ? speed : ""), 
				RenderManager::GetFontRef("menus"), 1);
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

			WidgetButton *buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
			canvas->Add("p10", new WidgetControllerMenuBack(0));

			canvas->Add("p8", new WidgetPlayerDisplay());
		}
	}
}


#endif

