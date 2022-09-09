#include "input.hpp"

#include <unordered_map>

#include "../events/event_system.hpp"
#include "../events/events_common.hpp"

#include <assert.h>

#include <GLFW/glfw3.h>

#include "input_handlers.hpp"
#include "../util/misc.hpp"
#include "../container/binding_container.hpp"
#include "../io/io_window.hpp"
#include <wge/core/logging.hpp>

namespace Rhythmic
{
	extern GLFWwindow *g_window;
	Rhythmic::InputDevice *g_keyboardDevice = 0;

	std::unordered_map<InputDeviceHandle, InputDevice> g_inputDevices;

	/*
	-----------------------------------
	Input Helper Functions
	-----------------------------------
	*/
	InputDevice *InputHelperGenDevice()
	{
		InputDeviceHandle handle = 0;

		// Find an empty valid handle. Two InputDevices CAN NOT HAVE THE SAME HANDLE
		auto found = g_inputDevices.begin();
		while ((found = g_inputDevices.find(handle)) != g_inputDevices.end())
			handle++;

		InputDevice device;
		device.handle = handle;

		memset(device.m_lastInputEvent.m_heldButtons, 0, sizeof(int) * INPUT_BUTTON_size);

		g_inputDevices.insert(std::make_pair(handle, device));
		return &g_inputDevices[handle];
	}

	/*
	-----------------------------------
	Input Functions
	-----------------------------------
	*/
	void input_glfw_key(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		/*
		Send the key input through the temporary hack
		*/
		Rhythmic::IO_Window_HandleKey(key, scancode, action, mods);

		/*
		Let's not respond to anything when the key press repeats. This happens when a key is held
		down and acts in a "rapid fire" kind of way. Kind of useless for Rhythmic
		*/
		if (action == GLFW_REPEAT)
			return;

		if (g_keyboardDevice == 0)
			return;

		//if (Rhythmic::InputIsListeningToText())
		//	return;

		InputGLFW &keyboardRaw = g_keyboardDevice->glfwControllerData;

		double timestamp = Util::GetTime();		// Timestamp
		Rhythmic::InputHandlerClear(g_keyboardDevice, timestamp);

		if (action != keyboardRaw.buttonsLast[key])
		{
			Rhythmic::InputRawButton rawButton = Rhythmic::InputRawButton();
			rawButton.keybutton = key;

			keyboardRaw.buttonsLast[key] = action;

			Rhythmic::InputHandlerKey(rawButton, (action == GLFW_PRESS));
		}

		//Rhythmic::InputHandlerDispatchKey();
	}

	void InputInit()
	{
		InputDevice keyboard;
		keyboard.type = DEVICE_TYPE_KEYBOARD;
		keyboard.handle = INPUT_HANDLE_KEYBOARD;
		memset(keyboard.m_lastInputEvent.m_heldButtons, 0, sizeof(int) * INPUT_BUTTON_size);
		g_inputDevices.insert(std::make_pair(INPUT_HANDLE_KEYBOARD, keyboard));

		g_keyboardDevice = Rhythmic::InputGetDevice(Rhythmic::INPUT_HANDLE_KEYBOARD);

		InputGLFW &keyboardRaw = g_keyboardDevice->glfwControllerData;
		//input_glfw_struct *mouseRaw = (input_glfw_struct*)mouse->_rawInput;

		keyboardRaw.joyID = -1;
		keyboardRaw.axisCount = 0;
		keyboardRaw.axisLast = 0;
		keyboardRaw.povCount = 0;
		keyboardRaw.povsLast = 0;
		keyboardRaw.buttonCount = GLFW_KEY_LAST;
		keyboardRaw.buttonsLast = new unsigned char[GLFW_KEY_LAST];
		memset(keyboardRaw.buttonsLast, 0, GLFW_KEY_LAST);

		//mouseRaw->joyID = -1;
		//mouseRaw->axisCount = 4; // X, Y, Scroll H, Scroll V
		//mouseRaw->axisLast = new float[4];
		//mouseRaw->povCount = 0;
		//mouseRaw->povsLast = 0;
		//mouseRaw->buttonCount = GLFW_MOUSE_BUTTON_LAST;
		//mouseRaw->buttonsLast = new unsigned char[GLFW_MOUSE_BUTTON_LAST];

		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			if (glfwJoystickPresent(i))
			{
				Rhythmic::InputDevice *device = Rhythmic::InputHelperGenDevice();
				device->type = Rhythmic::DEVICE_TYPE_CONTROLLER;
				//device.queues = Rhythmic::InputGetDefaultInputQueue();

				const char *sGuid = glfwGetJoystickGUID(i);

				LOG_INFO("Input (glfw_impl) \"{0}\" : {1}", glfwGetJoystickName(i), sGuid);

				device->guid = WGE::Util::GUID(std::string(sGuid));

				InputGLFW &s = device->glfwControllerData;
				s.joyID = i;

				int axisCount = 0;
				glfwGetJoystickAxes(i, &axisCount);

				device->axisCalibrations.resize(axisCount);
				device->axisButtonControl.resize(axisCount);
				for (int j = 0; j < axisCount; j++)
				{
					Rhythmic::InputAxisCalibration &cal = device->axisCalibrations[j];
					cal.min = SHRT_MIN;
					cal.max = SHRT_MAX;
				}


				// Get Axes
				s.axis = glfwGetJoystickAxes(s.joyID, &s.axisCount);
				s.axisLast = new float[s.axisCount];
				memcpy(s.axisLast, s.axis, sizeof(float) * s.axisCount);

				// Get Buttons
				s.buttons = glfwGetJoystickButtons(s.joyID, &s.buttonCount);
				s.buttonsLast = new unsigned char[s.buttonCount];
				memcpy(s.buttonsLast, s.buttons, s.buttonCount);
			}

			glfwSetKeyCallback(g_window, input_glfw_key);
		}

		glfwSetJoystickCallback([](int id, int e) {
			if (e == GLFW_CONNECTED)
			{
				Rhythmic::InputDevice *device = Rhythmic::InputHelperGenDevice();
				device->type = Rhythmic::DEVICE_TYPE_CONTROLLER;

				InputGLFW &s = device->glfwControllerData;
				s.joyID = id;

				int axisCount = 0;
				glfwGetJoystickAxes(id, &axisCount);

				device->axisCalibrations.resize(axisCount);
				device->axisButtonControl.resize(axisCount);
				for (int i = 0; i < axisCount; i++)
				{
					Rhythmic::InputAxisCalibration &cal = device->axisCalibrations[i];
					cal.min = SHRT_MIN;
					cal.max = SHRT_MAX;
				}

				const char *sGuid = glfwGetJoystickGUID(id);

				if (sGuid)
				{
					LOG_INFO("Input (glfw_impl) Connected \"{0}\" : {1}", glfwGetJoystickName(id), sGuid);
					device->guid = WGE::Util::GUID(std::string(sGuid));
				}
				else
				{
					LOG_INFO("Input (glfw_impl) Connected \"{0}\" with unknown guid!", glfwGetJoystickName(id));
					device->guid = WGE::Util::GUID();
				}

				const ContainerBinding *binding = BindingContainer::GetBinding(device->guid);
				if (binding)
					device->binding = binding->binding;
				else
					device->binding = Rhythmic::InputBinding();


				// Get Axes
				s.axis = glfwGetJoystickAxes(s.joyID, &s.axisCount);
				s.axisLast = new float[s.axisCount];
				memcpy(s.axisLast, s.axis, sizeof(float) * s.axisCount);

				// Get Buttons
				s.buttons = glfwGetJoystickButtons(s.joyID, &s.buttonCount);
				s.buttonsLast = new unsigned char[s.buttonCount];
				memcpy(s.buttonsLast, s.buttons, s.buttonCount);
			}
			});
	}
	void InputTerminate()
	{
		g_inputDevices.clear();
	}

	bool pollController(InputDevice *device)
	{
		InputGLFW &raw = device->glfwControllerData;
		bool dispatch = false;

		Rhythmic::InputRawButton rawButton;
		double timestamp = Util::GetTime();		// Timestamp
		Rhythmic::InputHandlerClear(device, timestamp);

		if (device->type == Rhythmic::DEVICE_TYPE_KEYBOARD)
		{
		}
		else if (device->type == Rhythmic::DEVICE_TYPE_MOUSE)
		{
		}
		else
		{
			if (!glfwJoystickPresent(raw.joyID))
				return true;

			glfwGetJoystickButtons(raw.joyID, &raw.buttonCount);
			for (int i = 0; i < raw.buttonCount; i++)
			{
				if (raw.buttons[i] != raw.buttonsLast[i])
				{
					rawButton = Rhythmic::InputRawButton();
					rawButton.button = i;
					raw.buttonsLast[i] = raw.buttons[i];
					Rhythmic::InputHandler(device, rawButton, raw.buttons[i] == GLFW_PRESS ? SHRT_MAX : SHRT_MIN);
					dispatch = true;
				}
			}
			glfwGetJoystickAxes(raw.joyID, &raw.axisCount);
			for (int i = 0; i < raw.axisCount; i++)
			{
				if (raw.axis[i] != raw.axisLast[i])
				{
					rawButton = Rhythmic::InputRawButton();
					rawButton.axis = i;
					float ax = raw.axis[i];
					raw.axisLast[i] = ax;
					Rhythmic::InputHandler(device, rawButton, ax > 0 ? ax * SHRT_MAX : -ax * SHRT_MIN); // Unfortunately, rawData must be an int to preserve raw data. So the float is multiplied by the max of short. Accuracy was lost from GLFW converting to float
					dispatch = true;
				}
			}

		}
		if (dispatch)
			Rhythmic::InputHandlerDispatch(device);
		return false;
	}

	void InputPoll()
	{
		auto controllerIndex = g_inputDevices.begin();
		while (controllerIndex != g_inputDevices.end())
		{
			bool undetectable = pollController(&controllerIndex->second);
			if (undetectable)
			{
				InputGLFW &raw = controllerIndex->second.glfwControllerData;

				if (raw.buttonsLast)
				{
					delete[] raw.buttonsLast;
					raw.buttonsLast = 0;
				}
				if (raw.axisLast)
				{
					delete[] raw.axisLast;
					raw.axisLast = 0;
				}

				InputLostEventData lostData;
				lostData.lostHandle = controllerIndex->first;

				EventDispatch(ET_INPUT_DEVICE_LOST, &lostData);

				controllerIndex = g_inputDevices.erase(controllerIndex);
			}
			else
				controllerIndex++;
		}
	}

	InputDevice *InputGetDevice(InputDeviceHandle handle)
	{
		if (handle < 0)
			return 0;
		
		auto device = g_inputDevices.find(handle);
		if (device != g_inputDevices.end())
			return &device->second;

		return 0;
	}

	int InputGetRawAxis(InputDevice *device, int axis)
	{
		InputGLFW &s = device->glfwControllerData;

		float ax = s.axis[axis];

		return (ax > 0 ? ax * SHRT_MAX : -ax * SHRT_MIN);
	}

	bool InputIsKeyDown(int key)
	{
		return false;
	}

	void InputSetAllBinding(const WGE::Util::GUID &guid, const ContainerBinding *binding)
	{
		for (auto it = g_inputDevices.begin(); it != g_inputDevices.end(); it++)
		{
			if (it->second.guid == guid)
			{
				it->second.binding = binding->binding;
				it->second.axisCalibrations = binding->calibrations;
			}
		}
	}
	static bool g_textBoxListen = false;
	void InputSetTextListen(bool listen)
	{
		g_textBoxListen = listen;
	}
	bool InputIsListeningToText()
	{
		return g_textBoxListen;
	}
}