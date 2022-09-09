#ifndef RHYTHMIC_WIDGET_PLAYER_STATS_HPP_
#define RHYTHMIC_WIDGET_PLAYER_STATS_HPP_

#include "../widget.hpp"

#include "text.hpp"
#include "tile.hpp"

#include <vector>

namespace Rhythmic
{
	class WidgetPlayerStats : public WidgetBase
	{
	public:
		WidgetPlayerStats(int player);
		virtual ~WidgetPlayerStats();

		void SetScrollOffset(glm::vec2 scroll);

		virtual void Update(float delta);
		virtual void Render(float interpolation);
		virtual void ProcessInput(InputEventData *input, int playerIndex);
	private:
		std::vector<WidgetText> m_texts;
		WidgetTile m_frame;

		char m_scroll;
		float m_maxContentYSize;

		glm::vec2 m_scrollOffset;

		int m_focusPlayer;

		bool m_drawStats;
	};
}

#endif

