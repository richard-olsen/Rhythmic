#ifndef RHYTHMIC_WIDGET_TEXT_BOX_HPP_
#define RHYTHMIC_WIDGET_TEXT_BOX_HPP_

#include "../widget.hpp"

#include "text.hpp"

#include <glm/vec2.hpp>

#include "../../events/event_system.hpp"

namespace Rhythmic
{
	struct WidgetTextBoxColors
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

	extern const WidgetTextBoxColors WIDGET_TEXT_BOX_COLORS_PLACEHOLDER;

	class WidgetTextBox: public WidgetBase
	{
	public:
		WidgetTextBox(const std::string &initialText = "", int labelAlign = 0, bool reactToMouseCoords = true);
		virtual ~WidgetTextBox();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void OnMouseMove(float x, float y) override;
		void OnMouseClick(int button, float x, float y) override;

		void SetOnTextChanged(std::function<void(const std::string &text)> func);
		void SetOnEnter(std::function<void(std::string &text)> func);

		void SetMaxLength(int amount);
		int GetMaxLength();

		void Select();
		void Deselect();

		bool IsSelected();

		void SetColor(const WidgetTextBoxColors &color);

		bool IsIntersectingPoint(float x, float y);

		void _OnText(void *data);

		void SetText(const std::string &text, int cursorPos = -1);

		WidgetText &GetLabel();
	private:
		WidgetTextBoxColors m_colors;

		EventListener_raii m_textListener;

		bool m_reactToMouseCoords;

		std::function<void(std::string &text)> m_onEnter;
		std::function<void(const std::string &text)> m_onTextChanged;

		WidgetText m_text;
		int m_alignLabel;

		bool m_requestingInput;

		bool m_hovered;
		bool m_isMouseOver;

		int m_cursorPosition; // Points to the position of where a character will be inserted
		int m_maxLength = INT_MAX;
	};
}

#endif

