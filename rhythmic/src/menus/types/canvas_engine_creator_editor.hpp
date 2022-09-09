#ifndef CANVAS_FACTORY_ENGINE_CREATOR_EDITOR_HPP_
#define CANVAS_FACTORY_ENGINE_CREATOR_EDITOR_HPP_

#include <vector>

#include "../canvas.hpp"
#include "../canvas_data.hpp"

#include "../widgets/button_field.hpp"
#include "../widgets/image.hpp"
#include "../widgets/tile.hpp"
#include "../widgets/controller_back_menu.hpp"
#include "../widgets/player_display.hpp"
#include "../widgets/text.hpp"
#include "../widgets/canvas_preview.hpp"
#include "../widgets/engine_editor.hpp"

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
		void CanvasCreateEngineCreatorEditor(Canvas *canvas, void *canvasData)
		{
			EngineEditorData *data = (EngineEditorData*) canvasData;

			canvas->Add("bg", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas(), false));

			WidgetTile *frame = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
			frame->SetScale(glm::vec2(0.2f, 0.35f));
			frame->SetPosition(glm::vec2(0.25f, 0.5f));
			canvas->Add("frame", frame);

			WidgetButtonField *buttonField = new WidgetButtonField(glm::vec4(0, 0, 0, 0.6f));
			buttonField->SetPosition(frame->GetPosition() - glm::vec2(0, frame->GetScale().y - 0.04f));
			buttonField->SetScale(frame->GetScale() - glm::vec2(0, -0.04f));
			canvas->Add("buttonField", buttonField);

			WidgetEngineEditor *editor = new WidgetEngineEditor(data, 0);
			canvas->Add("editor", editor); // Engine Editor

			canvas->Add("player_display", new WidgetPlayerDisplay());
			//canvas->Add("back", new WidgetControllerMenuBack(0, []() {}));

			WidgetButton *buttonBack = new WidgetButton("<", [editor]() {
				if (editor->GetPage() == 0)
				{
					if (!editor->IsChangesSaved()) MenuSystem::GetInstance()->DisplayConfirmationBox([]()
						{
							MenuSystem::GetInstance()->QueuePopMenu();
						}, "You have unsaved changes. Are you sure you want to quit?");
					else MenuSystem::GetInstance()->QueuePopMenu();
				}
				else
				{
					editor->ChangePage(-10);
				}
				return BUTTON_FINISHED;
			}, 1);
			buttonBack->SetExtents(glm::vec2(0.015f, 0.05f), glm::vec2(0.05f, 0.14f));
			canvas->Add("button_back", buttonBack);
		}
	}
}

#endif