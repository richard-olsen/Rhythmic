#include "widget.hpp"

namespace Rhythmic
{

	WidgetBase::WidgetBase() :
		m_isActive(false),
		m_isDisabled(false),
		m_position(0),
		m_canvas(0),
		m_scale(1),
		m_canvasChangeCallback(0),
		m_id(-1)
	{ }
	WidgetBase::~WidgetBase()
	{ }

	void WidgetBase::ProcessInput(InputEventData *input, int playerIndex)
	{ }

	void WidgetBase::Render(float interpolation) {}

	bool WidgetBase::IsActive() { return m_isActive; }
	bool WidgetBase::IsDisabled() { return m_isDisabled; }

	void WidgetBase::SetCanvas(Canvas *canvas) { m_canvas = canvas; if (m_canvasChangeCallback) m_canvasChangeCallback(canvas); }
	void WidgetBase::SetCanvasChangeCallback(std::function<void(Canvas*)> callback) { m_canvasChangeCallback = callback; }

	void WidgetBase::OnMouseMove(float x, float y) {}
	void WidgetBase::OnMouseClick(int button, float x, float y) {}
	void WidgetBase::OnScroll(float dx, float dy) {}

	void WidgetBase::OnCanvasActivated() {}
	void WidgetBase::OnCanvasDeactivated() {}

	void WidgetBase::SetPosition(const glm::vec2 &position) { m_position = position; }
	void WidgetBase::SetScale(const glm::vec2 &scale) { m_scale = scale; }
	void WidgetBase::SetExtents(const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
	{
		m_position = (topLeft + bottomRight) * 0.5f;
		m_scale = (bottomRight - topLeft) * 0.5f;
	}
	void WidgetBase::SetDisabled(bool disabled)
	{
		m_isDisabled = disabled;
	}

	void WidgetBase::SetID(int id) { m_id = id; }
	int WidgetBase::GetID() { return m_id; }

	const glm::vec2 &WidgetBase::GetPosition() { return m_position; }
	const glm::vec2 &WidgetBase::GetScale() { return m_scale; }
}
