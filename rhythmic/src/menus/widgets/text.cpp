#include "text.hpp"

#include "../widget_util.hpp"

#include "../../io/io_window.hpp"

#include <wge/core/logging.hpp>

namespace Rhythmic
{
	WidgetText::WidgetText(const std::string &text, const FontReference &font, int alignment, const glm::vec4 &color) :
		m_font(font),
		m_text(text),
		m_alignment(alignment),
		m_tagData(0),
		m_tagBool(0),
		m_tagInt(0),
		m_tagString(new std::string("")),
		m_color(color),
		m_cursorPosition(-1)
	{ }
	WidgetText::~WidgetText()
	{ }

	void WidgetText::Update(float delta)
	{

	}
	void WidgetText::TagIntAtEnd(int *i)
	{
		if (i)
		{
			m_tagData = 1;
			m_tagInt = i;
		}
		else
			TagNothingAtEnd();
	}
	void WidgetText::TagBoolAtEnd(int *j)
	{
		if (j)
		{
			m_tagData = 2;
			m_tagInt = j;
		}
		else
			TagNothingAtEnd();
	}
	void WidgetText::TagBoolAtEnd(bool *b)
	{
		if (b)
		{
			m_tagData = 4;
			m_tagBool = b;
		}
		else
			TagNothingAtEnd();
	}
	void WidgetText::TagStringAtEnd(std::string *k) {
		if (k != 0)
		{
			m_tagData = 3;
			m_tagString = k;
		}
		else
			TagNothingAtEnd();
	}
	void WidgetText::TagNothingAtEnd()
	{
		m_tagData = 0;
		m_tagInt = 0;
		m_tagBool = 0;
		m_tagString = 0;
	}

	void WidgetText::Render(float interpolation)
	{
		glm::vec2 correctedScale;
		WidgetUtil::AdjustRatio(correctedScale, IO_Window_GetWidth(), IO_Window_GetHeight());

		// Use half the scale since stb calculates it as if Rhythmic has double the coordinates
		// stb calculates it for coordinates going from -1 -> 1
		// the menu system goes from 0 -> 1
		// Perhaps this should be done internally?
		correctedScale *= m_scale * 0.5f;

		std::string t = m_text;

		// There is a problem with the way I'm doing this right now. If the text element still exists and the data it's referencing doesnt, the game will crash!
		if (m_tagData == 1)
			t += std::to_string(*m_tagInt);
		if (m_tagData == 2)
			t += *m_tagInt ? "Yes" : "No";
		if (m_tagData == 3)
			t += *m_tagString;
		if (m_tagData == 4)
			t += *m_tagBool ? "Yes" : "No";

		m_font->DrawString(t, m_position.x, m_position.y, correctedScale.x, correctedScale.y, m_color, m_alignment, m_cursorPosition);
	}

	std::string &WidgetText::GetText()
	{
		return m_text;
	}

	const std::string &WidgetText::GetText() const
	{
		return m_text;
	}

	void WidgetText::SetText(const std::string &text)
	{
		m_text = text;
	}

	void WidgetText::SetColor(const glm::vec4 &color)
	{
		m_color = color;
	}

	void WidgetText::SetCursorPos(int cursorPosition)
	{
		m_cursorPosition = cursorPosition;
	}
}

