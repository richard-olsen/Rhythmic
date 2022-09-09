#ifndef CANVAS_FACTORY_PLAYERS_HPP_
#define CANVAS_FACTORY_PLAYERS_HPP_

#include "../canvas.hpp"

#include "../widgets/canvas_preview.hpp"
#include "../widgets/button.hpp"
#include "../widgets/player_settings.hpp"

#include "../canvas_factory.hpp"
#include "../menu_system.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		void CanvasCreatePlayers(Canvas *canvas, void *canvasData)
		{
			WidgetCanvasPreview *canvasPreview = new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas());
			canvas->Add("prev", canvasPreview);

			WidgetTile *tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			canvas->Add("so38cx", tile);

			/*WidgetButton *button = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1, true);
			button->SetExtents(glm::vec2(0.01f, 0.01f), glm::vec2(0.1f, 0.1f));
			canvas->Add("back_button", button);*/

			WidgetButton *buttonBack = new WidgetButton("<", []()
				{
					// Force all players to stop editing their settings

					// TODO find a way to skip this and NOT back out if a player has unsaved settings (color, etc.)
					for (int i = 0; i < 4; i++)
					{
						ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(i);
						player->m_isEditingPersonalSettings = false;
						player->m_settingsPage = PLAYER_SETTINGS_PAGE_MAIN; // Make it set to the main page
					}
					MenuSystem::GetInstance()->QueuePopMenu();
					return BUTTON_FINISHED;
				}, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);

			WidgetPlayerSettings *playerSettings = new WidgetPlayerSettings();
			canvas->Add("player_settings", playerSettings);
		}
	}
}

#endif

