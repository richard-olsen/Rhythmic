#ifndef RHYTHMIC_WIDGET_PLAYER_SETTINGS_H_
#define RHYTHMIC_WIDGET_PLAYER_SETTINGS_H_

#include "../widget.hpp"

#include "button_field.hpp"

namespace Rhythmic
{
	/*
	The purpose of this widget is to make it easier to handle 4 different button fields for each player
	*/
	class WidgetPlayerSettings : public WidgetBase
	{
	public:
		WidgetPlayerSettings();
		virtual ~WidgetPlayerSettings();

		void Update(float delta) override;
		void Render(float interpolation) override;
		void ProcessInput(InputEventData *input, int playerIndex) override;

		void OnMouseMove(float x, float y) override;
		void OnMouseClick(int button, float x, float y) override;
		void OnScroll(float x, float y) override;
	private:
		void SetButtonsSettings(int index);
		void SetButtonsSelectInstrument(int index);
		void ClearButtons(int index);

		struct
		{
			WidgetButtonField buttonField;
			int page;
		} m_buttonFields[4];
	};
}

#endif

