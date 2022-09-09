#ifndef RHYTHMIC_CANVAS_CONFIRMATION_BOX_HPP_
#define RHYTHMIC_CANVAS_CONFIRMATION_BOX_HPP_

#include "../../canvas.hpp"

#include "../../widgets/button_field.hpp"
#include "../../widgets/button.hpp"
#include "../../widgets/text.hpp"
#include "../../widgets/image.hpp"
#include "../../widgets/canvas_preview.hpp"
#include "../../widgets/tile.hpp"
#include "../../widgets/button.hpp"
#include "../../widgets/controller_back_menu.hpp"

#include "../../../util/misc.hpp"

#include "../../../rendering/render_manager.hpp"

#include <string>

namespace Rhythmic
{
	void CanvasCreateConfirmation(Canvas* canvas, std::function<void()> onClick, const std::string& message)
	{
		canvas->Add("bg", new WidgetCanvasPreview(MenuSystem::GetInstance()->GetTopCanvas(), false));

		WidgetTile* tile = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0, 0, 0, 0.6f));
		canvas->Add("p4", tile);

		WidgetTile* tile2 = new WidgetTile(0, 0, RenderManager::GetSheet("ui_elements"));
		tile2->SetExtents(glm::vec2(0, 0.3f), glm::vec2(1.0f, 0.6f));
		canvas->Add("p6", tile2);

		WidgetText* tx = new WidgetText(message, RenderManager::GetFontRef("menus"), 1, glm::vec4(0.2f, 0.2f, 0.2f, 1));
		tx->SetScale(glm::vec2(0.02f));
		tx->SetPosition(glm::vec2(0.5f, 0.4f));

		canvas->Add("text", tx);

		WidgetButtonField* buttonField = new WidgetButtonField();
		buttonField->SetPosition(glm::vec2(0.5f, 0.475f));
		buttonField->SetScale(glm::vec2(0.25f, 0.4f));
		buttonField->AddButton(new WidgetButton("Yes", 
			[onClick]()
			{
				MenuSystem::GetInstance()->QueuePopMenu();
				onClick();
				return BUTTON_FINISHED;
			}));
		buttonField->AddButton(new WidgetButton("No",
			[]()
			{
				MenuSystem::GetInstance()->QueuePopMenu();
				return BUTTON_FINISHED;
			}));

		canvas->Add("back", new WidgetControllerMenuBack(0));
		canvas->Add("field", buttonField);
	}
}

#endif

