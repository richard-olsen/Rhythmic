#ifndef RHYTHMIC_WIDGET_TEXT_HPP_
#define RHYTHMIC_WIDGET_TEXT_HPP_

#include "../widget.hpp"

#include "../../rendering/font.hpp"

#include "glm/vec2.hpp"

namespace Rhythmic
{
	class WidgetText : public WidgetBase
	{
	public:
		WidgetText(const std::string &text, const FontReference &font, int alignment = 0, const glm::vec4 &color = glm::vec4(1, 1, 1, 1));
		virtual ~WidgetText();

		virtual void Update(float delta);
		virtual void Render(float interpolation);

		void TagIntAtEnd(int *i);
		void TagBoolAtEnd(int *j);
		void TagBoolAtEnd(bool* b);
		void TagStringAtEnd(std::string *k);
		void TagNothingAtEnd();

		std::string &GetText();
		const std::string &GetText() const;
		void SetText(const std::string &text);

		void SetColor(const glm::vec4 &color);

		void SetCursorPos(int cursorPosition);
	private:
		FontReference m_font;

		char m_tagData;
		bool *m_tagBool;
		int *m_tagInt;
		std::string *m_tagString;

		glm::vec4 m_color;

		int m_cursorPosition;

		std::string m_text;
		int m_alignment;
	};
}

#endif

