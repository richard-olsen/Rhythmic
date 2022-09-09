#ifndef RHYTHMIC_WIDGET_SONG_START_HPP_
#define RHYTHMIC_WIDGET_SONG_START_HPP_

#include "../widget.hpp"

namespace Rhythmic
{
	class WidgetSongStart : public WidgetBase
	{
	public:
		WidgetSongStart();
		virtual ~WidgetSongStart();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void IncreasePlayersReady();
		void DecreasePlayersReady();

	private:
		int m_readyPlayers;
		double m_time;
		bool m_threadFinished;
	};
}

#endif

