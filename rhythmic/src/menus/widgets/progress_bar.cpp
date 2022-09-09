#include "progress_bar.hpp"

#include "../../rendering/render_manager.hpp"

namespace Rhythmic
{
	WidgetProgressBar::WidgetProgressBar(float initialValue) :
		WidgetBase(),
		m_back(0, 0, RenderManager::GetSheet("ui_elements")),
		m_progress(0, 0, RenderManager::GetSheet("ui_elements"), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)),
		m_progressValue(initialValue)
	{
		SetCanvasChangeCallback([this](Canvas *canvas)
			{
				m_back.SetCanvas(canvas);
				m_progress.SetCanvas(canvas);
			});

	}
	WidgetProgressBar::~WidgetProgressBar()
	{ }

	void WidgetProgressBar::Update(float delta)
	{
		m_back.SetPosition(m_position);
		m_back.SetScale(m_scale);

		m_progress.SetPosition(m_position - glm::vec2(m_scale.x * (1.0f - m_progressValue), 0));
		m_progress.SetScale(glm::vec2(m_scale.x * m_progressValue, m_scale.y));
	}

	void WidgetProgressBar::Render(float interpolation)
	{
		m_back.Render(interpolation);
		m_progress.Render(interpolation);
	}

	void WidgetProgressBar::SetProgress(float progress)
	{
		m_progressValue = glm::clamp(progress, 0.0f, 1.0f);
	}
}