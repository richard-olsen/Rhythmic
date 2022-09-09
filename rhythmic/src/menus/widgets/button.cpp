#include "button.hpp"

#include "../../rendering/render_manager.hpp"
#include "../../io/io_window.hpp"

#include "button_field.hpp"

#include "../menu_system.hpp"

#include "../canvas.hpp"

#include <wge/core/logging.hpp>

namespace Rhythmic
{
	const WidgetButtonColors WIDGET_BUTTON_COLORS_PLACEHOLDER =
	{
		glm::vec4(0.0f, 0.0f, 0.0f, 0.6f),		// Normal
		glm::vec4(0.1f, 0.1f, 0.1f, 0.6f),		// Disabled
		glm::vec4(0.2f, 0.2f, 0.4f, 0.6f),		// Selected
		glm::vec4(0.1f, 0.1f, 0.2f, 0.6f),		// Disabled Selected
		glm::vec4(0.4f, 0.4f, 0.7f, 0.6f),		// Forced On

		glm::vec4(1,    1,    1,    1),						// Normal
		glm::vec4(0.6f, 0.6f, 0.6f, 1),						// Disabled
		glm::vec4(1,    1,    1,    1),						// Selected
		glm::vec4(0.6f, 0.6f, 0.6f, 1),						// Disabled Selected
		glm::vec4(1,    1,    1,    1)						// Forced On
	};

	WidgetButton::WidgetButton(const std::string &text, std::function<ButtonAction()> onClickFunction, int alignLabel, std::function<ButtonAction(int)> onScrollFunction, bool confirm) :
		WidgetBase(),
		m_hovered(false),
		m_isMouseOver(false),
		m_text(text, RenderManager::GetFontRef("menus"), alignLabel),
		onClick(onClickFunction),
		onScroll(onScrollFunction),
		m_alignLabel(alignLabel),
		m_requestingInput(false),
		m_colors(WIDGET_BUTTON_COLORS_PLACEHOLDER),
		m_forcedOn(false),
		m_forcedOff(false),
		m_confirm(confirm)
	{
	}
	WidgetButton::~WidgetButton()
	{
	}

	void WidgetButton::Update(float delta)
	{

	}
	void WidgetButton::Render(float interpolation)
	{
		SheetReference ui = RenderManager::GetSheet("ui_elements");
		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();
		ui->Bind();

		batcher->Clear();

		glm::vec4 &color = 
			m_isDisabled ? (m_hovered ? m_colors.disabledSelected : m_colors.disabled) :
			m_forcedOn ? m_colors.forcedSelect :
			m_forcedOff ? m_colors.disabled :
			m_hovered ? m_colors.selected :
			m_colors.normal;
		glm::vec4 &fontColor =
			m_isDisabled ? (m_hovered ? m_colors.textDisabledSelected : m_colors.textDisabled) :
			m_forcedOn ? m_colors.textForcedSelected :
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

	void WidgetButton::SetForceOn(bool forceOn) { m_forcedOn = forceOn; }
	void WidgetButton::SetForceOff(bool forceOff) { m_forcedOff = forceOff; }

	bool WidgetButton::GetForceOff() { return m_forcedOff; }
	bool WidgetButton::GetForceOn() { return m_forcedOn; }
	bool WidgetButton::IsSelected() { return m_hovered; }

	bool WidgetButton::IsIntersectingPoint(float x, float y)
	{
		glm::vec2 topLeft = m_position - m_scale;
		glm::vec2 botRight = m_position + m_scale;

		return x >= topLeft.x && x < botRight.x &&
			y >= topLeft.y && y < botRight.y;
	}

	void WidgetButton::Select()
	{
		m_hovered = true;
	}
	void WidgetButton::Deselect()
	{
		m_hovered = false;
	}

	void WidgetButton::SetOnClick(std::function<ButtonAction()> clickFunc)
	{
		onClick = clickFunc;
	}
	void WidgetButton::SetOnScroll(std::function<ButtonAction(int)> scrollFunc)
	{
		onScroll = scrollFunc;
	}


	void WidgetButton::SetColor(const WidgetButtonColors &color)
	{
		m_colors = color;
	}

	ButtonAction WidgetButton::OnClick()
	{
		if (onClick && !m_isDisabled)
		{
			if (m_confirm)
			{
				MenuSystem::GetInstance()->DisplayConfirmationBox(onClick, "Are You Sure?");
			} else return onClick();
		}
		return BUTTON_FINISHED;
	}

	ButtonAction WidgetButton::OnScroll(int value)
	{
		if (onScroll)
			return onScroll(value);
		return BUTTON_FINISHED;
	}

	void WidgetButton::OnMouseMove(float x, float y)
	{
		if (m_canvas == nullptr || !m_canvas->IsActive())
			return;

//		LOG_INFO("Mouse Coords: {0},{1} | {2},{3}", mData->x, mData->y, mData->identX, mData->identY);

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
	void WidgetButton::OnMouseClick(int button, float x, float y)
	{
		if (m_canvas == nullptr || !m_canvas->IsActive() && !m_isDisabled)
			return;

		if (m_hovered && m_isMouseOver && button == 0)
			OnClick();
	}

	WidgetText &WidgetButton::GetLabel()
	{
		return m_text;
	}
}

