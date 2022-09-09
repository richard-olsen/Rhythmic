#ifndef RHYTHMIC_WIDGET_PLAYER_PREP_HPP_
#define RHYTHMIC_WIDGET_PLAYER_PREP_HPP_

#include "../widget.hpp"

#include "../../chart/chart.hpp"

#include "tile.hpp"
#include "button_field.hpp"

namespace Rhythmic
{
	class WidgetSongStart;
	class WidgetPlayerPrep : public WidgetBase
	{
	public:
		WidgetPlayerPrep(Chart *chart, int playerIndex, WidgetSongStart *songStart);
		virtual ~WidgetPlayerPrep();

		void Update(float delta) override;
		void Render(float interpolation) override;
		void ProcessInput(InputEventData *input, int playerIndex) override;
		
		void OnScroll(float dx, float dy) override;
		void OnMouseMove(float x, float y) override;
		void OnMouseClick(int mouse, float x, float y) override;

		void ChangePage(int page);
		void SetPageInstrument();
		void SetPageDifficulty();
		void SetPageModifiers();
	private:
		Chart *m_chart;
		int m_playerIndex;
		int m_page;

		WidgetTile m_frame;
		WidgetButtonField m_buttons;

		WidgetSongStart *m_songStart;
	};
}

#endif

