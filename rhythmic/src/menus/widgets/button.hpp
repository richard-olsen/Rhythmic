#ifndef RHYTHMIC_WIDGET_BUTTON_HPP_
#define RHYTHMIC_WIDGET_BUTTON_HPP_

#include "../widget.hpp"

#include "text.hpp"

#include <glm/vec2.hpp>

#include "../../events/event_system.hpp"

namespace Rhythmic
{
	// TODO

	// Have button extend frame, to allow different widgets to overlay on the button such as icons and text

	enum ButtonAction
	{
		BUTTON_FINISHED,
		BUTTON_TOGGLE_SCROLL
	};

	class WidgetButtonField;

	struct WidgetButtonColors
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

	extern const WidgetButtonColors WIDGET_BUTTON_COLORS_PLACEHOLDER;

	class WidgetButton : public WidgetBase
	{
	public:
		WidgetButton(const std::string &text, std::function<ButtonAction()> clickFunc = 0, int labelAlign = 1, std::function<ButtonAction(int)> scrollFunc = 0, bool confirm = false);
		virtual ~WidgetButton();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void Select();
		void Deselect();
		void SetOnClick(std::function<ButtonAction()> clickFunc);
		void SetOnScroll(std::function<ButtonAction(int)> scrollFunc);

		void SetForceOn(bool forceOn);
		void SetForceOff(bool forceOff);

		bool GetForceOff();
		bool GetForceOn();

		bool IsSelected();

		void SetColor(const WidgetButtonColors &color);

		bool IsIntersectingPoint(float x, float y);

		void OnMouseMove(float x, float y) override;
		void OnMouseClick(int button, float x, float y) override;

		ButtonAction OnClick(); // Just a function to call the func pointer
		ButtonAction OnScroll(int value); // Just a function to call the func pointer

		WidgetText &GetLabel();
	private:
		std::function<ButtonAction()> onClick;
		std::function<ButtonAction(int)> onScroll;

		WidgetButtonColors m_colors;

		WidgetText m_text;
		int m_alignLabel;

		bool m_requestingInput;

		bool m_forcedOn;
		bool m_forcedOff;

		bool m_hovered;
		bool m_isMouseOver;

		bool m_confirm;
	};
}

#endif

