#ifndef RHYTHMIC_WIDGET_MENU_MUSIC_HPP_
#define RHYTHMIC_WIDGET_MENU_MUSIC_HPP_

#include "../widget.hpp"

#include "../../rendering/sheet.hpp"

#include "glm/vec2.hpp"

namespace Rhythmic
{
	class WidgetMenuMusic: public WidgetBase
	{
	public:
		WidgetMenuMusic();
		virtual ~WidgetMenuMusic();

		void Update(float delta) override;
		void Render(float interpolation) override;

		void OnCanvasActivated() override;
	};
}

#endif

