#include "canvas.hpp"

#include <cstring>

namespace Rhythmic
{
	Canvas::Canvas() :
		m_widgets(),
		m_isActive(false)
	{ }

	Canvas::~Canvas()
	{
		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
		{
			delete it->second;
		}
		m_widgets.clear();
	}

	void Canvas::Update(float delta)
	{
		if (!m_isActive)
			return;

		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
		{
			it->second->Update(delta);
		}
	}

	void Canvas::OnMouseMove(float x, float y)
	{
		if (!m_isActive)
			return;
		
		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
			it->second->OnMouseMove(x, y);
	}
	void Canvas::OnMouseClick(int button, float x, float y)
	{
		if (!m_isActive)
			return;

		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
			it->second->OnMouseClick(button, x, y);
	}
	void Canvas::OnScroll(float dx, float dy)
	{
		if (!m_isActive)
			return;

		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
			it->second->OnScroll(dx, dy);
	}

	void Canvas::ProcessInput(InputEventData *input, int playerIndex)
	{
		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
		{
			it->second->ProcessInput(input, playerIndex);
		}
	}

	void Canvas::Add(const std::string &widgetID, WidgetBase *widget)
	{
		auto it = FindWidget(widgetID);
		if (it == m_widgets.end())
		{
			widget->SetCanvas(this);
			m_widgets.push_back(std::make_pair(widgetID, widget));
		}
	}
	void Canvas::Remove(const std::string &widgetID)
	{
		auto it = FindWidget(widgetID);
		if (it != m_widgets.end())
		{
			delete it->second;
			m_widgets.erase(it);
		}
	}


	std::vector<std::pair<std::string, WidgetBase*>>::iterator Canvas::FindWidget(const std::string &widgetID)
	{
		auto it = m_widgets.begin();
		for (; it != m_widgets.end(); it++)
		{
			if (widgetID == it->first)
				return it;
		}
		return m_widgets.end();
	}

	void Canvas::SetActive(bool active)
	{
		m_isActive = active;
		if (active)
			for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
				it->second->OnCanvasActivated();
		else
			for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
				it->second->OnCanvasDeactivated();
	}
	bool Canvas::IsActive() { return m_isActive; }

	std::vector<std::pair<std::string, WidgetBase*>> &Canvas::GetWidgets() { return m_widgets; }

	int Canvas::GetType() { return m_type; }
	void Canvas::SetType(int type) { m_type = type; }
}

