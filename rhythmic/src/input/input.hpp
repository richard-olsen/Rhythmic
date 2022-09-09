#ifndef RHYTHMIC_INPUT_HPP_
#define RHYTHMIC_INPUT_HPP_

#include "input_structures.hpp"

namespace Rhythmic
{
	void InputInit();
	void InputTerminate();

	void InputPoll();

	/*
	Returns an InputDevice by index
	*/
	InputDevice *InputGetDevice(InputDeviceHandle handle);

	/*
	Returns the raw input of the axis
	*/
	int InputGetRawAxis(InputDevice *device, int axis);

	/*
	Returns a boolean that's true if the Input System thinks the key is down
	*/
	bool InputIsKeyDown(int key);

	/*
	Sets the bindings of all controllers with a given GUID
	*/
	void InputSetAllBinding(const WGE::Util::GUID &guid, const ContainerBinding *bindings);

	void InputSetTextListen(bool listen);
	bool InputIsListeningToText();
}

#endif

