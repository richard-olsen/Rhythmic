#include "frame.hpp"

namespace Rhythmic
{
	WidgetFrame::WidgetFrame(const std::string &text)
	{

	}
	WidgetFrame::~WidgetFrame()
	{
		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
		{
			delete it->second;
		}
		m_widgets.clear();
	}

	void WidgetFrame::Update(float delta)
	{
		for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
		{
			it->second->Update(delta);
		}
	}
	void WidgetFrame::Render(float interpolation)
	{
		for (auto i = m_widgets.begin(); i != m_widgets.end(); i++)
		{
			i->second->Render(interpolation);
		}
	}

	void WidgetFrame::Add(const char *widgetID, WidgetBase *widget)
	{
		auto it = m_widgets.find(widgetID);
		if (it == m_widgets.end())
			m_widgets.insert(std::pair<const char *, WidgetBase *>(widgetID, widget));
	}
}