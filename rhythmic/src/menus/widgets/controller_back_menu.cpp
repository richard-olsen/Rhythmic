#include "controller_back_menu.hpp"

#include "../../container/player_container.hpp"
#include "../menu_system.hpp"

namespace Rhythmic
{
	WidgetControllerMenuBack::WidgetControllerMenuBack(int playerIndexFocus, std::function<void()> exitCallback) :
		m_playerIndexFocus(playerIndexFocus),
		m_exitCallback(exitCallback)
	{ }
	WidgetControllerMenuBack::~WidgetControllerMenuBack()
	{ }

	void WidgetControllerMenuBack::Update(float delta)
	{ }
	void WidgetControllerMenuBack::Render(float interpolation)
	{ }
	void WidgetControllerMenuBack::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex != m_playerIndexFocus)
			return;

		if (input->m_buttonsPressed & (1 << INPUT_BUTTON_RED))
		{
			if (m_exitCallback)
				m_exitCallback();
			MenuSystem::GetInstance()->QueuePopMenu();
		}
	}
}