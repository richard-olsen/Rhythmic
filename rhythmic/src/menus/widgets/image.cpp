#include "image.hpp"

#include "../../rendering/render_manager.hpp"

#include "../../io/io_window.hpp"

#include "../widget_util.hpp"

namespace Rhythmic
{
	WidgetImage::WidgetImage(const IO_Image &image, int flags, bool useAsBackground, bool allowScaling) :
		m_allowScaling(allowScaling)
	{
		m_image.Create(&image, flags);
		m_invImageSize = glm::vec2((float)image.width, (float)image.height);

		m_invImageSize /= (m_invImageSize.x > m_invImageSize.y ? m_invImageSize.x : m_invImageSize.y);

		if (useAsBackground)
		{
			float winWidth = (float)IO_Window_GetWidth();
			float bgWidth = image.width;
			bgWidth = ((bgWidth <= winWidth ? bgWidth : winWidth) / winWidth); // Sets it correctly (so the bg will always fit the screen)

			m_scale = glm::vec2(bgWidth);
		}
	}
	WidgetImage::~WidgetImage()
	{
		m_image.Destroy();
	}

	void WidgetImage::SetImage(const IO_Image &image, int flags)
	{
		m_image.UpdateTexture(&image, flags);
	}

	void WidgetImage::Update(float delta) { }
	void WidgetImage::Render(float interpolation)
	{
		m_image.Bind();

		Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

		batcher->Clear();

		const glm::vec4 color = { 1,1,1,1 };

		glm::vec2 size;
		if (m_allowScaling)
		{
			WidgetUtil::AdjustRatio(size, (float)IO_Window_GetWidth(), (float)IO_Window_GetHeight());

			size *= m_scale * m_invImageSize;
		}
		else
		{
			size = m_scale;
		}

		batcher->AddVertex(m_position.x - size.x, m_position.y - size.y, 0, 0, 0, color.x, color.y, color.z, color.w);
		batcher->AddVertex(m_position.x + size.x, m_position.y - size.y, 0, 1, 0, color.x, color.y, color.z, color.w);
		batcher->AddVertex(m_position.x + size.x, m_position.y + size.y, 0, 1, 1, color.x, color.y, color.z, color.w);
		batcher->AddVertex(m_position.x + size.x, m_position.y + size.y, 0, 1, 1, color.x, color.y, color.z, color.w);
		batcher->AddVertex(m_position.x - size.x, m_position.y + size.y, 0, 0, 1, color.x, color.y, color.z, color.w);
		batcher->AddVertex(m_position.x - size.x, m_position.y - size.y, 0, 0, 0, color.x, color.y, color.z, color.w);

		batcher->Update();
		batcher->Draw();
	}
}