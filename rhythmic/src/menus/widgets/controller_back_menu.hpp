#ifndef RHYTHMIC_WIDGET_CONTROLLER_BACK_MENU_HPP_
#define RHYTHMIC_WIDGET_CONTROLLER_BACK_MENU_HPP_

// This widget allows players to hit the back button on their controller / keyboard to go back a menu without needing to press the back button

#include "../widget.hpp"

namespace Rhythmic
{
	class WidgetControllerMenuBack : public WidgetBase
	{
	public:
		WidgetControllerMenuBack(int playerIndexFocus, std::function<void()> exitCallback = 0);
		~WidgetControllerMenuBack();

		virtual void Update(float delta);
		virtual void Render(float interpolation);
		virtual void ProcessInput(InputEventData *input, int playerIndex);
	private:
		int m_playerIndexFocus;						// The player index to focus on
		std::function<void()> m_exitCallback;		// A callback function when the player exits
	};
}

#endif

