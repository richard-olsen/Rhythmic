#ifndef RHYTHMIC_WIDGET_SELECTABLE_HPP_
#define RHYTHMIC_WIDGET_SELECTABLE_HPP_

#include "../widget.hpp"

#include "text.hpp"

#include <glm/vec2.hpp>

#include "../../events/event_system.hpp"

namespace Rhythmic
{
	// TODO

	// Have button extend frame, to allow different widgets to overlay on the button such as icons and text

	struct WidgetSelectableColors
	{
		glm::vec4 normal;
		glm::vec4 disabled;
		glm::vec4 selected;
		glm::vec4 disabledSelected;
		glm::vec4 forcedSelect;

		glm::vec4 text;
		glm::vec4 textDisabled;
		glm::vec4 textSelected;
		glm::vec4 textDisabledSelected;
		glm::vec4 textForcedSelected;
	};

	extern const WidgetSelectableColors WIDGET_SELECTABLE_COLORS_PLACEHOLDER;

	class WidgetSelectable : public WidgetBase
	{
	public:
		WidgetSelectable(bool reactToMouseCoords = true);
		virtual ~WidgetSelectable();

		//virtual void Update(float delta);
		//virtual void Render(float interpolation);

		virtual void OnMouseMove(float x, float y);
		virtual void OnMouseClick(int button, float x, float y);

		void Select();
		void Deselect();

		bool IsSelected();

		void SetColor(const WidgetTextBoxColors &color);

		bool IsIntersectingPoint(float x, float y);
	private:
		WidgetSelectableColors m_colors;

		WidgetText m_text;
		int m_alignLabel;

		bool m_requestingInput;

		bool m_hovered;
		bool m_isMouseOver;
	};
}

#endif

