#ifndef RHYTHMIC_WIDGET_CANVAS_PREVIEW_HPP_
#define RHYTHMIC_WIDGET_CANVAS_PREVIEW_HPP_

#include "../widget.hpp"

#include "text.hpp"

#include <glm/vec2.hpp>

#include "../../events/event_system.hpp"

#include "../../rendering/framebuffer.hpp"

namespace Rhythmic
{
	class WidgetCanvasPreview : public WidgetBase
	{
	public:
		WidgetCanvasPreview(Canvas *canvas, bool doBlur = true); // Must be a pointer, WidgetCanvasPreview will NOT deallocate the canvas
		virtual ~WidgetCanvasPreview();

		void Update(float delta) override;
		void Render(float interpolation) override;
	private:
		void DrawFrame(float interpolation);

		bool m_hasDrawnFrame;

		Canvas *m_canvas;

		FrameBuffer m_frameBuffer;

		bool m_isBlur;
	};
}

#endif

