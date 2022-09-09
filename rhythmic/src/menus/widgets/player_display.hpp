#ifndef RHYTHMIC_WIDGET_PLAYER_NAME_DISPLAY_H_
#define RHYTHMIC_WIDGET_PLAYER_NAME_DISPLAY_H_

#include "../widget.hpp"

namespace Rhythmic
{
	class WidgetPlayerDisplay : public WidgetBase
	{
	public:
		WidgetPlayerDisplay();
		virtual ~WidgetPlayerDisplay();

		virtual void Update(float delta);
		virtual void Render(float interpolation);
		virtual void ProcessInput(InputEventData *input, int playerIndex);
	};
}

#endif

