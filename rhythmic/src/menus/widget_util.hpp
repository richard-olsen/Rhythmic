#ifndef RHYTHMIC_WIDGET_UTIL_HPP_
#define RHYTHMIC_WIDGET_UTIL_HPP_

#include <glm/vec2.hpp>

namespace Rhythmic
{
	namespace WidgetUtil
	{
		inline void AdjustRatio(glm::vec2 &out, float window_width, float window_height)
		{
			out = glm::vec2(1, window_width / window_height);
		}
	}
}

#endif

