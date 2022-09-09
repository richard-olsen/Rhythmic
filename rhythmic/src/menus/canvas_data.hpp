#ifndef RHYTHMIC_CANVAS_DATA_HPP_
#define RHYTHMIC_CANVAS_DATA_HPP_

#include "../input/input_structures.hpp"

#include "../container/engine/engine.hpp"
#include "widgets/button_field.hpp"

namespace Rhythmic
{
	class WidgetBindingRow;

	struct CanvasDataBindingOptions
	{
		InputRawButton rawButton;
		InputGameButton button;
		WidgetBindingRow *row;
	};

	struct CanvasDataAutoBinder
	{
		enum
		{
			AUTO_BINDER_DRUMS,
			AUTO_BINDER_GUITAR,
			AUTO_BINDER_KEYS,
		} binderType;

		InputDeviceHandle handle;
	};

	struct EngineEditorData
	{
		WGE::Util::GUID guid;
		WidgetButtonField* buttonField;
		int buttonIndex;
		bool isSaved = false;
		Engine engine;
	};
}

#endif

