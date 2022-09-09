#include "player_display.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../container/player_container.hpp"

#include "../../io/io_window.hpp"

#include "../menu_system.hpp"
#include "../canvas_factory.hpp"

#include "../../game/stage/stage.hpp"

namespace Rhythmic
{
	WidgetPlayerDisplay::WidgetPlayerDisplay()
	{

	}
	WidgetPlayerDisplay::~WidgetPlayerDisplay()
	{

	}

	void WidgetPlayerDisplay::Update(float delta)
	{
		if (!m_canvas->IsActive())
			return;

		for (unsigned int i = 0; i < 4; i++)
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(i);
			if (player->m_playerID == 5)
				continue;

			//if (player->m_inputDevice.hitDataButton[Input::BUTTON_START].pressed)
			//	player->m_isEditingPersonalSettings = true;

			if (player->m_isEditingPersonalSettings)
			{
				MenuSystem::GetInstance()->QueuePushMenu(CanvasFactory::CreateCanvas(CANVAS_FACTORY_PLAYERS_SETTINGS));
				break;
			}
		}
	}
	void WidgetPlayerDisplay::ProcessInput(InputEventData *input, int playerIndex)
	{
		bool mayActivateNewPlayer = true;

		mayActivateNewPlayer =
			(StageSystem::GetInstance()->IsActive() && PlayerContainer::GetPlayerLostConnection() != 5) ||
			!StageSystem::GetInstance()->IsActive();

		if ((input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_START)))
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByInputDevice(input->device);
			if (player == 0)
			{
				if (mayActivateNewPlayer)
					PlayerContainer::AssignNewPlayer(input->device);
			}
			else
			{
				if (player->m_playerID != 5 && !StageSystem::GetInstance()->IsActive())
					player->m_isEditingPersonalSettings = true;
			}
		}
	}
	void WidgetPlayerDisplay::Render(float interpolation)
	{
		if (!m_canvas->IsActive())
			return;

		FontReference font = RenderManager::GetFontRef("menus");

		const float fontScale = 0.01f;
		float fontScaleY = fontScale * ((float)IO_Window_GetWidth() / (float)IO_Window_GetHeight());

		for (unsigned int i = 0; i < 4; i++)
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(i);
			if (player->m_playerID != 5)
			{
				font->DrawString(("Player " + std::to_string(i+1)), (0.25f * i) + 0.125f, 1.0f - fontScaleY * 2, fontScale, fontScaleY, glm::vec4(1, 1, 1, 1), 1);
			}
			else
				font->DrawString("Press Start", (0.25f * i) + 0.125f, 1.0f - fontScaleY * 2, fontScale, fontScaleY, glm::vec4(1, 1, 1, 1), 1);
		}
	}
}

