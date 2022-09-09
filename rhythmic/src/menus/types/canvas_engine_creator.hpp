#ifndef CANVAS_FACTORY_ENGINE_CREATOR_HPP_
#define CANVAS_FACTORY_ENGINE_CREATOR_HPP_

#include <vector>
#include <random>

#include "../canvas.hpp"
#include "../canvas_data.hpp"

#include "../widgets/button_field.hpp"
#include "../widgets/image.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"
#include "../widgets/player_display.hpp"
#include "../widgets/text.hpp"

#include <wge/core/logging.hpp>

#include "../../rendering/render_manager.hpp"

#include "../menu_system.hpp"

#include "../../util/settings.hpp"
#include "../../util/thread_pool.hpp"
#include "../../util/misc.hpp"

#include "../../container/song_container.hpp"
#include "../../container/engine/engine_container.hpp"

#include "../../container/engine/engine.hpp"

#include "../../io/io_window.hpp"

namespace Rhythmic
{
	namespace FactoryCanvas
	{
		// Global variables for pushing to menu
		EngineEditorData data;

		void CanvasCreateEngineCreator(Canvas *canvas, void *canvasData)
		{
			IO_Image background; // Image variable
			IO_LoadImage(Util::GetExecutablePath() + "/game_data/images/backgrounds/space_01.png", &background); // Load background image into background var reference
			WidgetImage *bg = new WidgetImage(background, Rendering::TEXTURE_FLAG_CLAMP | Rendering::TEXTURE_FLAG_FILTER_LINEAR, true);
			IO_FreeImageData(&background);

			canvas->Add("background", bg); // Adds background widget to canvas

			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(1, 1, 1, 0.7f));
			frame->SetScale(glm::vec2(0.2f, 0.35f));
			frame->SetPosition(glm::vec2(0.25f, 0.5f));
			canvas->Add("frame", frame);

			WidgetButtonField *engineButtons = new WidgetButtonField(glm::vec4(0, 0, 0, 0.6f));
			engineButtons->SetPosition(frame->GetPosition() - glm::vec2(0, frame->GetScale().y - 0.04f));
			engineButtons->SetScale((frame->GetScale() * glm::vec2(1, 1.88f)) - glm::vec2(0, 0.04f));//(frame->GetScale() * glm::vec2(1,2)) - glm::vec2(0, -0.04f));

			engineButtons->AddButton(new WidgetButton("Create New Engine", [engineButtons] 
				{
					Engine engine;
					data.engine = engine;
					data.guid = WGE::Util::GUID();
					data.buttonField = engineButtons;
					data.buttonIndex = engineButtons->GetButtons()->size() - 1;
					data.isSaved = false;

					// Queue the push with the data
					MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
					return BUTTON_FINISHED;
				}, 0));

			std::map<WGE::Util::GUID, Engine> *engines = EngineContainer::GetEngines();
			std::vector<Engine> engineSort;

			for (auto swag = engines->begin(); swag != engines->end(); swag++) // Create built-in Engines buttons so they are viewable but not editable.
			{
				WGE::Util::GUID guid = swag->first;
				Engine engine = swag->second;

				if (engine.m_isCustom) continue;

				int size = engineButtons->GetButtons()->size();

				engineButtons->AddButton(new WidgetButton(engine.m_name, [engineButtons, guid, size, engine]
					{
						EngineEditorData data = { guid, engineButtons, size, true, engine };
						MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
						return BUTTON_FINISHED;
					}, 0));
			}

			// Begin engine sorting by name

			for (auto it = engines->begin(); it != engines->end(); it++)
			{
				engineSort.push_back(it->second);
			}

			std::sort(engineSort.begin(), engineSort.end(), [](Engine first, Engine second) 
			{
				return first.m_name.compare(second.m_name);
			});

			engines->clear();

			for (int i = 0; i < engineSort.size(); i++)
			{
				engines->insert(std::make_pair(engineSort[i].m_guid, engineSort[i]));
			}

			// End of engine sorting

			// Add custom engine buttons
			for (auto swag = engines->begin(); swag != engines->end(); swag++)
			{
				WGE::Util::GUID guid = swag->first;
				Engine engine = swag->second;

				if (!engine.m_isCustom) continue;

				int size = engineButtons->GetButtons()->size();

				engineButtons->AddButton(new WidgetButton(engine.m_name, [engineButtons, guid, size, engine]
					{
						EngineEditorData data = {guid, engineButtons, size, true, engine};
						MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_ENGINE_CREATOR_EDITOR, &data));
						return BUTTON_FINISHED;
					}, 0));
			}

			canvas->Add("engineButtons", engineButtons);

			canvas->Add("player_display", new WidgetPlayerDisplay());
			canvas->Add("back", new WidgetControllerMenuBack(0, []() {}));

			WidgetButton *buttonBack = new WidgetButton("<", []() { MenuSystem::GetInstance()->QueuePopMenu(); return BUTTON_FINISHED; }, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif