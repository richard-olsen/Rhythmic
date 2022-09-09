#include "canvas_preview.hpp"

#include "../canvas.hpp"

#include "../../io/io_window.hpp"
#include "../../rendering/vertex_batcher.hpp"
#include "../../rendering/render_manager.hpp"

namespace Rhythmic
{
	WidgetCanvasPreview::WidgetCanvasPreview(Canvas *canvas, bool doBlur) :
		m_canvas(canvas),
		m_isBlur(doBlur),
		m_hasDrawnFrame(false)
	{
		m_frameBuffer.Create(IO_Window_GetWidth(), IO_Window_GetHeight());
	}

	WidgetCanvasPreview::~WidgetCanvasPreview()
	{ }

	void WidgetCanvasPreview::Update(float delta) 
	{ }

	void WidgetCanvasPreview::Render(float interpolation)
	{
		DrawFrame(interpolation);

		if (m_isBlur)
		{
			// Render the framebuffer
			Rendering::VertexBatcher *batcher = RenderManager::GetBatcher();

			batcher->AddVertex(0, 0, 0, 0, 1, 1, 1, 1, 1);
			batcher->AddVertex(1, 0, 0, 1, 1, 1, 1, 1, 1);
			batcher->AddVertex(1, 1, 0, 1, 0, 1, 1, 1, 1);

			batcher->AddVertex(1, 1, 0, 1, 0, 1, 1, 1, 1);
			batcher->AddVertex(0, 1, 0, 0, 0, 1, 1, 1, 1);
			batcher->AddVertex(0, 0, 0, 0, 1, 1, 1, 1, 1);

			m_frameBuffer.Bind();
			batcher->Update();
			batcher->Draw();
		}
	}

	void WidgetCanvasPreview::DrawFrame(float interpolation)
	{
		if (m_isBlur && m_frameBuffer.IsValid() && m_canvas)
		{
			if (m_hasDrawnFrame)
				return;
			/*
			Render the blur
			*/
			m_frameBuffer.BindTarget();

			RenderManager::UseShader(SHADER_TYPE_SCREEN);

			auto widgets = m_canvas->GetWidgets();

			bool active = m_canvas->IsActive();
			m_canvas->SetActive(false);

			for (auto i = widgets.begin(); i != widgets.end(); i++)
			{
				i->second->Render(interpolation);
			}

			RenderManager::ApplyBlur(m_frameBuffer, 2.0f);
			RenderManager::ApplyBlur(m_frameBuffer, 1.0f);

			FrameBuffer::BindGlobalTarget();
			
			m_hasDrawnFrame = true;
		}
		else
		{
			if (m_canvas)
			{
				auto widgets = m_canvas->GetWidgets();

				for (auto i = widgets.begin(); i != widgets.end(); i++)
				{
					i->second->Render(interpolation);
				}
			}
		}
	}
}

