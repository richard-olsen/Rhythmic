#ifndef RHYTHMIC_MENU_BASE_HPP_
#define RHYTHMIC_MENU_BASE_HPP_

#include "widget.hpp"

#include <vector>
#include <string>

#include "../input/input_structures.hpp"

namespace Rhythmic
{
	class Canvas
	{
	public:
		Canvas();
		~Canvas();

		void Update(float delta);
		
		void Add(const std::string &widgetID, WidgetBase *widget);
		void Remove(const std::string &widgetID);

		void ProcessInput(InputEventData *input, int playerIndex);

		void OnMouseMove(float x, float y);
		void OnMouseClick(int button, float x, float y);
		void OnScroll(float dx, float dy);

		void SetActive(bool active);
		bool IsActive();

		std::vector<std::pair<std::string, WidgetBase*>> &GetWidgets();
		std::vector<std::pair<std::string, WidgetBase*>>::iterator FindWidget(const std::string &widgetID);

		int GetType();
		void SetType(int type);
	private:
		std::vector<std::pair<std::string,WidgetBase*>> m_widgets;

		bool m_isActive;

		int m_type;
	};
}
#endif

