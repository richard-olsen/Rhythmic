#include "button_field.hpp"

#include "../../rendering/glad/glad.h"
#include "../../io/io_window.hpp"

#include "../canvas.hpp"
#include "../canvas_factory.hpp"

#include "../../game/globals.hpp"

#include "../../container/player_container.hpp"
#include "../../input/input.hpp"

#include <wge/core/logging.hpp>

#define BORDER_SCALE 2.333333f

namespace Rhythmic
{
	WidgetButtonField::WidgetButtonField(const glm::vec4 &color, int playerIndexFocus, bool allowButtonScroll) :
		m_buttons(0),
		m_selected(0),
		m_allowScrolling(true),
		m_buttonRequestedScrollEvent(false),
		m_color(WIDGET_BUTTON_COLORS_PLACEHOLDER),
		m_playerIndexFocus(playerIndexFocus),
		m_offset(0),
		m_heldScrollDown(false),
		m_heldScrollUp(false),
		m_buttonYScale(0.03f),
		m_allowButtonScroll(allowButtonScroll),
		WidgetBase()
	{
		auto callback = [this](Canvas* canvas)
		{
			for (size_t i = 0; i < m_buttons.size(); i++)
				m_buttons[i]->SetCanvas(canvas);
		};

		SetCanvasChangeCallback(callback);
	}
	WidgetButtonField::~WidgetButtonField()
	{
		ClearButtons();
	}

	void WidgetButtonField::SetOrigin(const glm::vec2 &position)
	{
		// TODO
		// Iterate through each button to adjust position
	}
	void WidgetButtonField::SetSize(const glm::vec2 &size)
	{

	}
	void WidgetButtonField::SetButtonYScale(float scale)
	{
		m_buttonYScale = scale;

		for (int i = 0; i < m_buttons.size(); i++)
		{
			WidgetButton *button = m_buttons[i];

			button->SetScale(glm::vec2(button->GetScale().x, m_buttonYScale));
		}
	}

	void WidgetButtonField::SetScrollOffset(const glm::vec2 &offset) // Set scroll offset
	{
		if (m_offset != offset && m_allowScrolling)
		{
			// Clamp 
			m_offset = glm::clamp(offset, -glm::vec2(0, glm::max(0.0f, m_maxContentYSize - m_scale.y)), glm::vec2(0));


			for (size_t i = 0; i < m_buttons.size(); i++)
				m_buttons[i]->SetPosition(m_position + glm::vec2(m_offset.x, (m_buttonYScale * BORDER_SCALE) * i + m_offset.y));
		}
	}

	void WidgetButtonField::SetColor(const WidgetButtonColors &color)
	{
		m_color = color;

		for (size_t i = 0; i < m_buttons.size(); i++)
			m_buttons[i]->SetColor(color);
	}

	glm::vec2 WidgetButtonField::GetScrollOffset() // Retrieve current scroll offset
	{
		return m_offset;
	}

	/*
	Order dependent
	First button added is given the ID of zero
	*/
	void WidgetButtonField::AddButton(WidgetButton *button)
	{
		assert(button);
		// Set size and position based on size of current button list

		size_t buttonVectorSize = m_buttons.size();

		glm::vec2 targetPos = glm::vec2(0, (m_buttonYScale * BORDER_SCALE) * buttonVectorSize);

		button->SetPosition(m_position + targetPos + m_offset);
		button->SetScale(glm::vec2(m_scale.x, m_buttonYScale));
		button->SetCanvas(m_canvas);
		button->SetColor(m_color);
		if (buttonVectorSize == 0)
			button->Select();

		m_buttons.push_back(button);

		float max = targetPos.y;
		m_maxContentYSize = (button->GetScale().y * 0.5f) + max;
	}
	void WidgetButtonField::ClearButtons()
	{
		for (int i = 0; i < m_buttons.size(); i++)
		{
			delete m_buttons[i];
		}
		m_buttons.clear();
		m_selected = 0;
		m_buttonRequestedScrollEvent = false;

		m_maxContentYSize = 0;
	}

	std::vector<WidgetButton*> *WidgetButtonField::GetButtons() { return &m_buttons; }

	void WidgetButtonField::Update(float delta)
	{
		if (m_buttons.empty())
			return;

		const float speed = 3.0f; // Change to adjust speed

		//for (size_t i = 0; i < m_buttons.size(); i++)
		//	m_buttons[i]->Update(update);
		// Since the button update function is empty, there is no need to call update on all of them

		ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(m_playerIndexFocus);

		if (m_heldScrollDown && scrollTimer <= 0)
		{
			scrollTimer = 0.15f;
			
			if (m_buttonRequestedScrollEvent)
				m_buttons[m_selected]->OnScroll(-1);
			else if (m_selected < m_buttons.size() - 1)
				Select(m_selected + 1);
		}

		if (m_heldScrollUp && scrollTimer <= 0)
		{
			scrollTimer = 0.15f;

			if (m_buttonRequestedScrollEvent)
				m_buttons[m_selected]->OnScroll(1);
			else if (m_selected > 0)
				Select(m_selected - 1);
		}

		if (m_heldScrollUp || m_heldScrollDown)
			scrollTimer -= speed * delta; // Change to adjust scroll speed
	}
	
	void WidgetButtonField::ProcessInput(InputEventData *input, int playerIndex)
	{
		if (playerIndex != m_playerIndexFocus)
			return;

		if (input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_DOWN))
		{
			if (m_buttonRequestedScrollEvent)
				m_buttons[m_selected]->OnScroll(-1);
			else
				Select(m_selected + 1);

			m_heldScrollDown = true;
			m_heldScrollUp = false;

			scrollTimer = 1.0f;
		}
		if (input->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_DOWN))
			m_heldScrollDown = false;

		if (input->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_UP))
		{
			if (m_buttonRequestedScrollEvent)
				m_buttons[m_selected]->OnScroll(1);
			else
				Select(m_selected - 1);

			m_heldScrollUp = true;
			m_heldScrollDown = false;

			scrollTimer = 1.0f;
		}
		if (input->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_UP))
			m_heldScrollUp = false;

		if (input->m_buttonsPressed & ButtonToFlag( INPUT_BUTTON_GREEN))
			DoButtonAction();
	}

	void WidgetButtonField::Render(float interpolation)
	{
		glEnable(GL_SCISSOR_TEST);

		glm::vec2 pos = glm::vec2(IO_Window_GetWidth(), IO_Window_GetHeight());
		glm::vec2 sca = pos * m_scale;
		pos *= m_position;

		float height = (0.03 * IO_Window_GetHeight());

		glScissor(
			pos.x - sca.x, IO_Window_GetHeight() - (pos.y + sca.y + height),
			sca.x * 2, sca.y + height * 2
		);

		for (size_t i = 0; i < m_buttons.size(); i++)
		{
			if (IsInsideField(m_buttons[i]->GetPosition(), m_buttons[i]->GetScale()))
				m_buttons[i]->Render(interpolation);
		}


		glDisable(GL_SCISSOR_TEST);
	}

	bool WidgetButtonField::IsInsideField(const glm::vec2 &pos, const glm::vec2 &scale)
	{
		glm::vec2 tl = pos - scale;
		glm::vec2 br = pos + scale;

		return (br.x > (m_position.x - m_scale.x) && br.y > (m_position.y - 0.03f) && tl.x < (m_position.x + m_scale.x) && tl.y < (m_position.y + m_scale.y + 0.03f));
	}

	bool WidgetButtonField::IsPointInsideField(float x, float y)
	{
		return (x > (m_position.x - m_scale.x) && x < (m_position.x + m_scale.x) && y < (m_position.y + m_scale.y + 0.03f) && y > (m_position.y - 0.03f));
	}

	void WidgetButtonField::OnScroll(float dx, float dy)
	{
		if (!m_canvas->IsActive())
			return;

		bool scroll = IsPointInsideField(m_lastRecordedMousePos.x, m_lastRecordedMousePos.y);

		if (m_buttonRequestedScrollEvent && scroll)
		{
			m_buttons[m_selected]->OnScroll(glm::clamp((int)dy, -1, 1));
			return;
		}

		if (dy != 0)
		{
			if (scroll)
			{
				SetScrollOffset(GetScrollOffset() + glm::vec2(0, dy * 0.05f));
				OnMouseMove(m_lastRecordedMousePos.x, m_lastRecordedMousePos.y);
			}
			else if (m_allowButtonScroll)
				Select(m_selected + (dy < 0 ? 1 : -1));
		}
	}
	void WidgetButtonField::OnMouseMove(float x, float y)
	{
		if (!m_canvas->IsActive() || m_buttonRequestedScrollEvent)
			return;

		m_lastRecordedMousePos.x = x;
		m_lastRecordedMousePos.y = y;

		if (!IsPointInsideField(x, y)) // No need to continue and update the buttons when the mouse isn't even inside the field
			return;

		for (size_t i = 0; i < m_buttons.size(); i++)
		{
			WidgetButton *button = m_buttons[i];
			if (!IsInsideField(button->GetPosition(), button->GetScale())) // If the button is visible inside the range
				continue;

			if (button->IsIntersectingPoint(x, y))
			{
				if (i != m_selected)
				{
					bool lastValue = m_allowScrolling;
					m_allowScrolling = false;
					Select(i);
					m_allowScrolling = lastValue;
					/*m_buttons[m_selected]->Deselect();
					button->Select();
					m_selected = i;*/
				}
			}
		}
	}

	void WidgetButtonField::SetAllowScroll(bool allowScrolling)
	{
		m_allowScrolling = allowScrolling;
	}
	void WidgetButtonField::SetAllowButtonScroll(bool allowButtonScroll)
	{
		m_allowButtonScroll = allowButtonScroll;
	}

	void WidgetButtonField::SetFocusPlayer(int index)
	{
		m_playerIndexFocus = index;
	}

	void WidgetButtonField::OnMouseClick(int button, float x, float y)
	{
		if (!m_canvas->IsActive() || m_buttons.empty())
			return;

		if (IsPointInsideField(x, y))
		{
			if (m_buttons[m_selected]->IsIntersectingPoint(x, y) && button == 0)
			{
				DoButtonAction();
			}
		}
	}

	void WidgetButtonField::DoButtonAction()
	{
		if (m_buttons.empty())
			return;

		ButtonAction action = m_buttons[m_selected]->OnClick();

		switch (action)
		{
		case BUTTON_TOGGLE_SCROLL:
			m_buttonRequestedScrollEvent = !m_buttonRequestedScrollEvent;
			break;
			break;
		}
	}

	void WidgetButtonField::Select(int button)
	{
		if (m_buttons.empty())
			return;

		if (m_selected != button)
			m_buttons[m_selected]->Deselect();
		m_selected = button;
		
		if (m_selected < 0)
			m_selected = 0;
		if (m_selected >= m_buttons.size())
			m_selected = m_buttons.size() - 1;

		m_buttons[m_selected]->Select();

		if (m_canvas && m_canvas->GetType() == CANVAS_FACTORY_SONG_SELECTION) {
			Global_SetSongButtonSelected(button);
		}

		if (m_allowScrolling) // Fix this scrolling
		{
			glm::vec2 pos = m_buttons[m_selected]->GetPosition();

			//if (!IsInsideField(pos, m_buttons[m_selected]->GetScale()))
			//{
			//	glm::vec2 scroll = glm::vec2(0, -(pos.y - m_scale.y) + m_offset.y);
			//	SetScrollOffset(scroll);
			//}

			float center = m_position.y + m_scale.y * 0.5f;

			if (pos.y != center)
			{
				float dif = center - pos.y;
				glm::vec2 scroll = glm::vec2(0, dif + m_offset.y);
				SetScrollOffset(scroll);
			}
		}
	}

	void WidgetButtonField::ForceSelect(int button)
	{
		for (int i = 0; i < m_buttons.size(); i++)
			m_buttons[i]->Deselect();
		Select(button);
	}

	int WidgetButtonField::CurrentlySelected()
	{
		return m_selected;
	}

	WidgetButton *WidgetButtonField::Get(int button)
	{
		assert(button >= 0);
		assert(button < m_buttons.size());
		return m_buttons[button];
	}

	std::vector<WidgetButton*>::iterator WidgetButtonField::Begin()
	{
		return m_buttons.begin();
	}
	std::vector<WidgetButton*>::iterator WidgetButtonField::End()
	{
		return m_buttons.end();
	}
}