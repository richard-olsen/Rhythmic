#ifndef RHYTHMIC_WIDGET_HPP_
#define RHYTHMIC_WIDGET_HPP_

#include "../events/event_system.hpp"

#include "../input/input_structures.hpp"

#include <glm/vec2.hpp>

namespace Rhythmic
{
	class Canvas;

	class WidgetBase
	{
	public:
		WidgetBase();
		virtual ~WidgetBase();

		virtual void Update(float delta) = 0;
		virtual void Render(float interpolation);
		/*
		Called whenever the menu system receives any input from the input system
		*/
		virtual void ProcessInput(InputEventData *input, int playerIndex);
		
		/*
		Called whenever the menu system receives a mouse movement event
		*/
		virtual void OnMouseMove(float x, float y);

		/*
		Called whenever the menu system receives a mouse down event
		*/
		virtual void OnMouseClick(int button, float x, float y);

		/*
		Called whenever the menu system receives a mouse scrolling event
		*/
		virtual void OnScroll(float dx, float dy);

		/*
		This is called anytime the parent canvas becomes activated.
		Like if it's canvas is pushed onto the menu system stack,
		or if it's canvas becomes activated again when a player left another menu
		*/
		virtual void OnCanvasActivated();

		/*
		This is called when ever the canvas changes, or canvas is destroyed
		*/
		virtual void OnCanvasDeactivated();


		bool IsActive();
		bool IsDisabled();
		void SetDisabled(bool disabled);

		void SetCanvas(Canvas *canvas);

		void SetPosition(const glm::vec2 &position);
		void SetScale(const glm::vec2 &scale);
		void SetExtents(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);

		void SetID(int id);
		int GetID();

		const glm::vec2 &GetPosition();
		const glm::vec2 &GetScale();
	protected:
		bool m_isActive;
		bool m_isDisabled;

		int m_id;

		Canvas *m_canvas;

		void SetCanvasChangeCallback(std::function<void(Canvas*)> callback);
		std::function<void(Canvas*)> m_canvasChangeCallback;

		glm::vec2 m_position;
		glm::vec2 m_scale;
	};
}

#endif

