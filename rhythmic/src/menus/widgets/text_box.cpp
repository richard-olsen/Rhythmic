#include "text_box.hpp"

#include "../../rendering/render_manager.hpp"
#include "../../io/io_window.hpp"

#include "button_field.hpp"

#include "../canvas.hpp"

#include <wge/core/logging.hpp>

#include "../../io/io.hpp"

#include "../../input/input.hpp"

namespace Rhythmic
{
	const WidgetTextBoxColors WIDGET_TEXT_BOX_COLORS_PLACEHOLDER =
	{
		glm::vec4(0.0f, 0.0f, 0.0f, 0.6f),		// Normal
		glm::vec4(0.1f, 0.1f, 0.1f, 0.6f),		// Disabled
		glm::vec4(0.2f, 0.2f, 0.4f, 0.6f),		// Selected
		glm::vec4(0.1f, 0.1f, 0.2f, 0.6f),		// Disabled Selected
		glm::vec4(0.4f, 0.4f, 0.7f, 0.6f),		// Forced On

		glm::vec4(1,    1,    1,    1),			// Normal
		glm::vec4(0.6f, 0.6f, 0.6f, 1),			// Disabled
		glm::vec4(1,    1,    1,    1),			// Selected
		glm::vec4(0.6f, 0.6f, 0.6f, 1),			// Disabled Selected
		glm::vec4(1,    1,    1,    1)			// Forced On
	};

	WidgetTextBox::WidgetTextBox(const std::string &text, int alignLabel, bool reactToMouseCoords) :
		WidgetBase(),
		m_hovered(false),
		m_isMouseOver(false),
		m_text(text, RenderManager::GetFontRef("menus"), alignLabel),
		m_alignLabel(alignLabel),
		m_requestingInput(false),
		m_colors(WIDGET_TEXT_BOX_COLORS_PLACEHOLDER),
		m_textListener(ET_TEXT, CREATE_MEMBER_CB(WidgetTextBox::_OnText)),
		m_cursorPosition(0),
		m_onEnter(0),
		m_onTextChanged(0),
		m_reactToMouseCoords(reactToMouseCoords)
	{
		m_text.SetCursorPos(m_cursorPosition);
	}
	WidgetTextBox::~WidgetTextBox()
	{
	}

	void WidgetTextBox::Update(float delta)
	{
		if (m_cursorPosition > m_text.GetText().size())
			m_cursorPosition = m_text.GetText().size();
		if (m_cursorPosition < 0)
			m_cursorPosition = 0;

		// Make a timer to flicker the cursor pos
		if (m_hovered)
			m_text.SetCursorPos(m_cursorPosition);
		else
			m_text.SetCursorPos(-1);
	}

	void WidgetTextBox::_OnText(void *data)
	{
		if (!m_hovered)
			return;

		TextData *tData = (TextData*) data;

		std::string addString;
		std::string &text = m_text.GetText();
		if (tData->c == 0) // Most likely a command from the keyboard Ctrl + V or Backspace
		{
			if (tData->buttons.pressedEnter)
			{
				if (m_onEnter)
				{
					std::string copy = text;
					m_onEnter(text);
					if (copy != text)
						m_cursorPosition = text.size();
				}
			}

			bool left = false;
			if (tData->buttons.pressedBackspace)
			{
				if (m_cursorPosition > 0)
					text.erase(m_cursorPosition - 1, 1);
				left = true;
			}

			if (tData->buttons.pressedDelete)
			{
				if (m_cursorPosition < text.size())
					text.erase(m_cursorPosition, 1);
			}

			if (tData->buttons.pressedLeft || left)
			{
				m_cursorPosition--;
				if (m_cursorPosition < 0)
					m_cursorPosition = 0;
			}
			if (tData->buttons.pressedRight)
			{
				m_cursorPosition++;
				if (m_cursorPosition > text.size())
					m_cursorPosition = text.size();
			}

			if (tData->buttons.downModCtrl)
			{
				if (tData->buttons.paste)
				{
					IO_Clipboard clipboard;
					addString += clipboard;

					text.insert(m_cursorPosition, addString);
					m_cursorPosition += addString.size();
					if (m_onTextChanged)
						m_onTextChanged(text);
				}
			}
		}
		else
		{
			if (text.length() + 1 > GetMaxLength()) return; // Is new text amount bigger than specified size?

			addString += (char) tData->c;
			text.insert(m_cursorPosition, addString);
			if (m_onTextChanged)
				m_onTextChanged(text);

			m_cursorPosition += addString.size();
		}
		m_text.SetCursorPos(m_cursorPosition);
	}

	void WidgetTextBox::SetOnTextChanged(std::function<void(const std::string &text)> func) { m_onTextChanged = func; }
	void WidgetTextBox::SetOnEnter(std::function<void(std::string &text)> func) { m_onEnter = func; }

	void WidgetTextBox::SetMaxLength(int amount) { m_maxLength = amount; }
	int WidgetTextBox::GetMaxLength() { return m_maxLength; }

	void WidgetTextBox::Render(float interpolation)
	{
		SheetReference ui = RenderManager::GetSheet("ui_elements");
		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();
		ui->Bind();

		batcher->Clear();

		glm::vec4 &color =
			m_isDisabled ? (m_hovered ? m_colors.disabledSelected : m_colors.disabled) :
			m_hovered ? m_colors.selected :
			m_colors.normal;
		glm::vec4 &fontColor =
			m_isDisabled ? (m_hovered ? m_colors.textDisabledSelected : m_colors.textDisabled) :
			m_hovered ? m_colors.textSelected :
			m_colors.text;

		ui->AddSpriteScaled(0, 0, 1, 1, m_position, m_scale, color);

		batcher->Update();
		batcher->Draw();

		if (m_alignLabel == 1)
			m_text.SetPosition(m_position);
		else
			m_text.SetPosition(m_position - glm::vec2(m_scale.x - (m_scale.y * 0.35f), 0));

		m_text.SetScale(glm::vec2(m_scale.y * 0.7f));
		m_text.SetColor(fontColor);
		if (m_requestingInput)
		{
			// Use a different way maybe?
			std::string old = m_text.GetText();			// Grab a copy of the original text
			std::string &label_text = m_text.GetText();	// Returns a reference to the label's text

			label_text.clear();
			label_text.append("> " + old + " <");		// Set it to "> Option <" to signify the button is listening to the user

			m_text.Render(interpolation);

			label_text.clear();
			label_text.append(old);						// Reset the text to what it was before
		}
		else
			m_text.Render(interpolation);
	}

	bool WidgetTextBox::IsSelected() { return m_hovered; }

	bool WidgetTextBox::IsIntersectingPoint(float x, float y)
	{
		glm::vec2 topLeft = m_position - m_scale;
		glm::vec2 botRight = m_position + m_scale;

		return x >= topLeft.x && x < botRight.x &&
			y >= topLeft.y && y < botRight.y;
	}

	void WidgetTextBox::SetText(const std::string &text, int cursorPos)
	{
		m_text.GetText() = text;
		if (cursorPos < 0)
			m_cursorPosition = text.size();
		else
			m_cursorPosition = glm::min((size_t)cursorPos, text.size());
	}

	void WidgetTextBox::Select()
	{
		m_hovered = true;
		InputSetTextListen(true);
	}
	void WidgetTextBox::Deselect()
	{
		m_hovered = false;
		InputSetTextListen(false);
	}

	void WidgetTextBox::SetColor(const WidgetTextBoxColors &color)
	{
		m_colors = color;
	}

	void WidgetTextBox::OnMouseMove(float x, float y)
	{
		if (m_canvas == nullptr || !m_canvas->IsActive() || !m_reactToMouseCoords)
			return;

		if (IsIntersectingPoint(x, y))
		{
			Select();
			m_isMouseOver = true;
		}
		else
		{
			Deselect();
			m_isMouseOver = false;
		}
	}
	void WidgetTextBox::OnMouseClick(int button, float x, float y)
	{
		if (m_canvas == nullptr || !m_canvas->IsActive() && !m_isDisabled)
			return;

		if (m_hovered && m_isMouseOver && button == 0)
		{
			// enable / disable text box
		}
	}

	WidgetText &WidgetTextBox::GetLabel()
	{
		return m_text;
	}
}

