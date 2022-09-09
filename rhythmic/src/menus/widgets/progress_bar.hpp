#ifndef RHYTHMIC_WIDGET_PROGRESS_BAR_HPP_
#define RHYTHMIC_WIDGET_PROGRESS_BAR_HPP_

#include "../widget.hpp"
#include "tile.hpp"

namespace Rhythmic
{
	class WidgetProgressBar : public WidgetBase
	{
	public:
		WidgetProgressBar(float initialValue = 0);
		virtual ~WidgetProgressBar();

		virtual void Update(float delta);

		virtual void Render(float interpolation);

		void SetProgress(float progress);
	protected:
		float m_progressValue; // 0%->100% or 0.0 -> 1.0
	private:
		WidgetTile m_progress;
		WidgetTile m_back;
	};

}

#endif

