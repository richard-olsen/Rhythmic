#ifndef RHYTHMIC_WIDGET_FRAME_HPP_
#define RHYTHMIC_WIDGET_FRAME_HPP_

#include "../widget.hpp"

#include <glm/vec2.hpp>

#include <map>
#include <string>

namespace Rhythmic
{
	class WidgetFrame : public WidgetBase
	{
	public:
		WidgetFrame(const std::string &text);
		virtual ~WidgetFrame();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void Add(const char *widgetID, WidgetBase *widget);
	private:
		std::map<const char*, WidgetBase*> m_widgets;
	};
}

#endif

