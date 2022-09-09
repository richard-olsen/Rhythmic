#ifndef RHYTHMIC_INPUT_HANDLERS_HPP_
#define RHYTHMIC_INPUT_HANDLERS_HPP_

#include "input_structures.hpp"

#include <functional>

namespace Rhythmic
{
	/*
	Returns calibrated axis data
	*/
	float InputHandlerGetCalibratedState(int data, InputAxisCalibration &calibration);

	/*
	Clears and begins a new state for the input handler. To prepare it before sending out the event
	*/
	void InputHandlerClear(InputDevice *device, double timestamp);

	/*
	Handles raw data putting them into a nice structure to send out on the event
	*/
	void InputHandler(InputDevice *device, const InputRawButton &button, int rawData);

	/*
	Dispatches the event down the event system for instruments and menus to deal with
	*/
	void InputHandlerDispatch(InputDevice *device);

	/*
	Handles the raw key data
	*/
	void InputHandlerKey(const InputRawButton &button, int active);

	/*
	Dispatches the keys when they have been pressed or released
	*/
	void InputHandlerDispatchKey();

	/*
	Intercepts the input handler. Any button pressed (excluding any axes by default) by any input device having a default queue will trigger the function once

	Once the function is called, the handler resets and no longer calls the given function
	*/
	void InterceptHandler(std::function<void(InputDevice *device, const InputRawButton &button, int rawData)> func, bool sendAllRawValues = false);

	/*
	Returns true if the input handler is being intercepted
	*/
	bool InputHandlerBeingIntercepted();
}

#endif

