#ifndef RHYTHMIC_WIDGET_TILE_HPP_
#define RHYTHMIC_WIDGET_TILE_HPP_

#include "../widget.hpp"

#include "../../rendering/sheet.hpp"

#include "glm/vec2.hpp"

namespace Rhythmic
{
	class WidgetTile : public WidgetBase
	{
	public:
		WidgetTile(int tile_x, int tile_y, const SheetReference &font, const glm::vec4 &color = glm::vec4(1,1,1,1));
		virtual ~WidgetTile();

		virtual void Update(float delta);
		virtual void Render(float interpolation);
	private:
		SheetReference m_sheet;

		int m_tileX;
		int m_tileY;
		glm::vec4 m_color;
	};
}

#endif

