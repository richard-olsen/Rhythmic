#include "input_handlers.hpp"
#include "input.hpp"

#include "../events/event_system.hpp"
#include "../container/player_container.hpp"

#include <limits>

#include <cstring>

namespace Rhythmic
{
	std::function<void(InputDevice*,const InputRawButton &,int)> g_interceptFunction;
	bool g_sendAllRawValues;

	// Inserting and Get implementation for binding structures

	int InputBindingsContains(InputBinding *bind, const InputRawButton &rawButton)
	{
		for (int index = 0; index < bind->size(); index++)
		{
			if ((*bind)[index].first.keybutton == rawButton.keybutton)
				return index;
		}

		return -1;
	}

	void InputBindingsAddButton(InputBinding *bind, const InputRawButton &rawButton, InputGameButton gameButton)
	{
		auto it = std::find_if(bind->begin(), bind->end(), [rawButton](const InputBinder &binder) { return binder.first == rawButton; });//InputBindingsFind(bind, rawButton);
		if (it == bind->end())
		{
			std::vector<InputGameButton> buttons; buttons.push_back(gameButton);
			bind->push_back(InputBinder(rawButton, buttons));
		}
		else
		{
			// Prevents the same button / axis being bound twice to a gameButton
			for (auto gameIt = it->second.begin(); gameIt != it->second.end(); gameIt++)
				if (*gameIt == gameButton)
					return;

			it->second.push_back(gameButton);
		}
	}
	void InputBindingsRemoveButton(InputBinding *bind, const InputRawButton &rawButton, InputGameButton gameButton)
	{
		// InputBinder = pair of InputRawButton, and Vector of game buttons

		auto it = std::find_if(bind->begin(), bind->end(), [rawButton](const InputBinder &binder) { return binder.first == rawButton; });
		if (it != bind->end())
		{
			std::vector<InputGameButton> &boundButtons = it->second;
			// Prevents the same button / axis being bound twice to a gameButton
			for (auto gameIt = boundButtons.begin(); gameIt != boundButtons.end(); gameIt++)
			{
				if (*gameIt == gameButton)
				{
					boundButtons.erase(gameIt);
					break;
				}
			}

			// Remove the binder if there are no actively bound buttons
			if (boundButtons.empty())
				bind->erase(it);
		}
	}

	float InputHandlerGetCalibratedState(int data, InputAxisCalibration &calibration)
	{
		if (calibration.min < calibration.max)
		{
			if (data < calibration.min)
				data = calibration.min;
			if (data > calibration.max)
				data = calibration.max;

			int size = calibration.max - calibration.min;
			data -= calibration.min;
			return (float)data / (float)size;
		}
		else if (calibration.max < calibration.min)
		{
			data = -data;
			InputAxisCalibration t;
			t.max = -calibration.max;
			t.min = -calibration.min;

			if (data < t.min)
				data = t.min;
			if (data > t.max)
				data = t.max;

			int size = t.min - t.max;
			data -= t.max;
			return 1.0f - abs((float)data / (float)size);
		}
		return 0;
	}


	//void QueueAll(InputDevice *device, const InputRawButton &button, int data)
	//{
	//	if (device->queues == 0)
	//		return;

	//	auto it = std::find_if(device->binding.begin(), device->binding.end(), [button](const InputBinder &binder) { return binder.first == button; });
	//	if (it == device->binding.end())
	//		return;

	//	std::vector<InputGameButton> &buttons = it->second;
	//	for (int i = 0; i < buttons.size(); i++)
	//	{
	//		InputQueueData queueData;
	//		queueData.button = buttons[i];
	//		queueData.rawButton = button;
	//		queueData.device = device;
	//		queueData.rawState = data;
	//		queueData.timestamp = 0;// Fix later
	//		if (RAWB_IS_AXIS(button))
	//		{
	//			queueData.state = InputHandlerGetCalibratedState(data, device->axisCalibrations[button.axis]);
	//		}
	//		else
	//		{
	//			if (data > 0)
	//				queueData.state = 1.0f;
	//			else
	//				queueData.state = 0.0f;
	//		}

	//		device->queues->push(queueData);
	//	}
	//}

	/*void HandleBasicController(InputDevice *device, const InputRawButton &button, int rawData)
	{
		QueueAll(device, button, rawData);
	}*/

	void InputHandlerClear(InputDevice *device, double timestamp)
	{
		if (g_interceptFunction)
			return;

		device->m_lastInputEvent.timestamp = timestamp;
		device->m_lastInputEvent.m_buttonsPressed = 0;
		device->m_lastInputEvent.m_buttonsReleased = 0;
	}

	void InputHandler(InputDevice *device, const InputRawButton &button, int rawData)
	{
		/*
		Systems may intercept the input handler, allowing for functionality like a Binding system
		*/
		if (g_interceptFunction)
		{
			// Sometimes, a system doesn't want all the values
			if (button.axis < 0 || g_sendAllRawValues)
			{
				g_interceptFunction(device, button, rawData);
				return;
			}
		}

		/*
		Every button will be treated as an axis
		and will be tested their state
		Buttons will either be 0 or 1
		Axis will be in between 0 and 1
		*/
		float state = 0;

		if (button.button >= 0 || button.keybutton >= 0 || button.midi >= 0)
			state = rawData > 0 ? 1.0f : 0.0f;

		// Control axis flow
		// If one axis is down, it will control all game buttons it's bound to,
		// but can only control it once at a time. Otherwise, things will go weird
		// This forces the axis test to correctly add or remove from a game buttons
		// knowledge of how many physical buttons are down
		int axisButtonControl = 0;

		if (RAWB_IS_AXIS(button))
		{
			state = InputHandlerGetCalibratedState(rawData, device->axisCalibrations[button.axis]);

			if (state >= INPUT_AXIS_ACTIVATION && !device->axisButtonControl[button.axis])
			{
				axisButtonControl = 1;
				device->axisButtonControl[button.axis] = true;
			}
			else if (state < INPUT_AXIS_ACTIVATION && device->axisButtonControl[button.axis])
			{
				axisButtonControl = 2;
				device->axisButtonControl[button.axis] = false;
			}
		}

		/*
		Find the axis/button inside the bindings
		*/
		auto buttonBindings = std::find_if(device->binding.begin(), device->binding.end(), [button](const InputBinder &binder) { return binder.first == button; });
		
		if (buttonBindings != device->binding.end())
		{
			// Loop through each game button the physical button is bound to
			for (auto j = buttonBindings->second.begin(); j != buttonBindings->second.end(); j++)
			{
				std::pair<int, float> &axisMax = device->m_lastInputEvent.axis[(*j)];

				// Update axis value in the axis list

				axisMax.first = rawData;
				axisMax.second = state;

				/*
				Axis are treated a little different. To keep them from constantly adding 1 or removing 1 to
				the physical button count
				*/
				if (RAWB_IS_AXIS(button))
				{
					if (axisButtonControl == 1)
					{
						device->m_lastInputEvent.m_buttonsPressed |= ButtonToFlag((*j));
						device->m_lastInputEvent.m_heldButtons[*j]++;
					}
					else if (axisButtonControl == 2)
					{
						int &physicalButton = device->m_lastInputEvent.m_heldButtons[*j];
						physicalButton--;
						if (physicalButton <= 0)
						{
							physicalButton = 0;
							device->m_lastInputEvent.m_buttonsReleased |= ButtonToFlag((*j));
						}
					}
				}
				else
				{
					if (state >= INPUT_AXIS_ACTIVATION)
					{
						device->m_lastInputEvent.m_buttonsPressed |= ButtonToFlag((*j));
						device->m_lastInputEvent.m_heldButtons[*j]++;
					}
					else
					{
						int &physicalButton = device->m_lastInputEvent.m_heldButtons[*j];
						physicalButton--;
						if (physicalButton <= 0)
						{
							physicalButton = 0;
							device->m_lastInputEvent.m_buttonsReleased |= ButtonToFlag((*j));
						}
					}
				}
			}
		}
	}

	void InputHandlerKey(const InputRawButton &button, int active)
	{
		if (g_interceptFunction)
		{
			g_interceptFunction(InputGetDevice(INPUT_HANDLE_KEYBOARD), button, active ? SHRT_MAX : SHRT_MIN);
			return;
		}

		InputDevice *keyDevice = InputGetDevice(INPUT_HANDLE_KEYBOARD);
		InputHandlerClear(keyDevice, keyDevice->m_lastInputEvent.timestamp);
		InputHandler(keyDevice, button, active);
		if (!InputIsListeningToText())
			InputHandlerDispatch(keyDevice);

		for (int playerIndex = 0; playerIndex < 4; playerIndex++)
		{
			ContainerPlayer *player = PlayerContainer::GetPlayerByIndex(playerIndex);
			if (player->m_inputHandle == INPUT_HANDLE_KEYBOARD)
				continue;

			InputDevice *device = InputGetDevice(player->m_inputHandle);
			if (device == 0)
				continue;

			InputHandlerClear(device, device->m_lastInputEvent.timestamp);

			InputHandler(device, button, active ? SHRT_MAX : SHRT_MIN);

			if (!InputIsListeningToText())
				InputHandlerDispatch(device);
		}
	}
	void InputHandlerDispatchKey()
	{
		if (g_interceptFunction)
			return;


	}

	void InputHandlerDispatch(InputDevice *device)
	{
		if (g_interceptFunction)
			return;

		device->m_lastInputEvent.device = device;

		EventDispatch(ET_INPUT, &device->m_lastInputEvent);
	}

	bool InputHandlerBeingIntercepted()
	{
		return (bool)g_interceptFunction;
	}

	void InterceptHandler(std::function<void(InputDevice*device, const InputRawButton &button, int rawData)> func, bool sendAllRawValues)
	{
		g_interceptFunction = func;
		g_sendAllRawValues = sendAllRawValues;
	}
}