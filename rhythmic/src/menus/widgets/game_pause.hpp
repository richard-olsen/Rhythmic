#ifndef RHYTHMIC_WIDGET_PAUSE_FUNC_HPP_
#define RHYTHMIC_WIDGET_PAUSE_FUNC_HPP_

#include "../widget.hpp"

#include "button_field.hpp"

#include "../../game/player.hpp"

namespace Rhythmic
{
	class WidgetPracticeToSectionSelection : public WidgetBase
	{
	public:
		WidgetPracticeToSectionSelection();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		bool m_didRedirect;
	};

	class WidgetPauseFunc : public WidgetBase
	{
	public:
		WidgetPauseFunc(WidgetButtonField *pauseButtons);
		virtual ~WidgetPauseFunc();

		virtual void Update(float delta);
		virtual void Render(float interpolation);
		virtual void ProcessInput(InputEventData *input, int playerIndex);

		void SetPracticeRedirect(WidgetPracticeToSectionSelection *sectionSelection);
	private:
		Player *m_pausedPlayer;

		WidgetButtonField *m_pauseCanvasButtonField;
		WidgetPracticeToSectionSelection *m_toSectionRedirect;

		bool m_wantSectionChange;
		bool m_allowPausing;

		void EmplaceButtons();
	};
}

#endif

