#ifndef RHYTHMIC_WIDGET_ENGINE_EDITOR_HPP_
#define RHYTHMIC_WIDGET_ENGINE_EDITOR_HPP_

#include "../widget.hpp"
#include "../../container/engine/engine.hpp"
#include "../canvas_data.hpp"

#include "../../chart/chart.hpp"

#include "tile.hpp"
#include "button_field.hpp"
#include "text.hpp"
#include "text_box.hpp"

namespace Rhythmic
{
	class WidgetEngineEditor : public WidgetBase
	{
	public:
		WidgetEngineEditor(EngineEditorData *data, int playerIndex);
		virtual ~WidgetEngineEditor();

		virtual void OnCanvasActivated();
		virtual void OnMouseClick(int button, float x, float y);
		virtual void OnMouseMove(float x, float y);
		virtual void OnScroll(float dx, float dy);

		virtual void Update(float delta);
		virtual void Render(float interpolation);
		virtual void ProcessInput(InputEventData *input, int playerIndex);

		int GetPage();

		bool IsChangesSaved();

		void ChangePage(int page);
		void SetPageInstrumentSelector();
		void SetPageGuitar();
		void SetPageDrums();
		void SetPageKeys();

		void ReinitializeButtons(WidgetButtonField *buttons);
	private:
		Engine m_editorEngine;

		int m_playerIndex;
		int m_page;
		int m_buttonIndex;

		WidgetTextBox m_nameBox;
		WidgetTextBox m_authorBox;

		WidgetTile m_frame;

		WidgetButtonField m_buttons;
		WidgetButtonField *m_engineButtons;

		bool m_isSaved;
		bool m_isChangesSaved;
	};
}

#endif

