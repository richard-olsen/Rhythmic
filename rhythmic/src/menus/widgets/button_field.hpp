#ifndef BUTTON_FIELD_H_
#define BUTTON_FIELD_H_

#include "../widget.hpp"
#include "button.hpp"

#include <glm/vec2.hpp>

namespace Rhythmic
{
	class WidgetButtonField : public WidgetBase
	{
	public:
		WidgetButtonField(const glm::vec4 &color = glm::vec4(0,0,0,0.6f), int playerIndexFocus = 0, bool allowButtonScroll = true); // playerIndexFocus tells the button field which player to focus on for controller input
		~WidgetButtonField();

		void SetButtonYScale(float scale);

		void SetOrigin(const glm::vec2 &position);
		void SetSize(const glm::vec2 &size);
		void SetScrollOffset(const glm::vec2 &offset); // Set scroll offset
		void SetAllowScroll(bool allowScrolling);
		void SetAllowButtonScroll(bool allowButtonScroll);

		glm::vec2 GetScrollOffset(); // Retrieve current scroll offset

		/*
		Order dependent
		First button added is given the ID of zero
		*/
		void AddButton(WidgetButton *button);
		void ClearButtons();
		std::vector<WidgetButton*> *GetButtons();

		void Update(float delta) override;
		void Render(float interpolation) override;
		void ProcessInput(InputEventData *input, int playerIndex) override;

		bool IsInsideField(const glm::vec2 &pos, const glm::vec2 &scale);
		bool IsPointInsideField(float x, float y);

		void SetColor(const WidgetButtonColors &color);

		void OnMouseMove(float x, float y) override;
		void OnMouseClick(int button, float x, float y) override;
		void OnScroll(float dx, float dy) override;

		void SetFocusPlayer(int index);

		void DoButtonAction();

		void Select(int button);
		void ForceSelect(int button);

		int CurrentlySelected();

		WidgetButton *Get(int button);

		std::vector<WidgetButton*>::iterator Begin();
		std::vector<WidgetButton*>::iterator End();
	protected:
		std::vector<WidgetButton*> m_buttons;
		int m_selected;

		bool m_allowScrolling;

		bool m_heldScrollUp;
		bool m_heldScrollDown;

		glm::vec2 m_lastRecordedMousePos;

		WidgetButtonColors m_color;

	private:
		float m_buttonYScale;
		float m_maxContentYSize;

		glm::vec2 m_offset;

		bool m_allowButtonScroll; // Allows the scroll wheel to select other buttons

		int m_playerIndexFocus;

		bool m_buttonRequestedScrollEvent;

		float scrollTimer;
	};
}

#endif

