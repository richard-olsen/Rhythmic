#ifndef RHYTHMIC_INPUT_STRUCTURES_HPP_
#define RHYTHMIC_INPUT_STRUCTURES_HPP_

#include <queue>
#include <map>
#include <wge/util/guid.hpp>
#include "input_game.hpp"

#define RAWB_IS_AXIS(raw) (raw.axis >= 0)
#define RAWB_IS_BUTTON(raw) (raw.button >= 0)
#define RAWB_IS_KEY(raw) (raw.keybutton >= 0)
#define RAWB_IS_MIDI(raw) (raw.midi >= 0)

#define BUTTON_HELD(device, button) (device->m_lastInputEvent.m_heldButtons[button] > 0)

namespace Rhythmic
{

	/*
	--------------------------
	Input Button items
	--------------------------
	*/

	enum DeviceType
	{
		DEVICE_TYPE_CONTROLLER,
		DEVICE_TYPE_GUITAR,
		DEVICE_TYPE_DRUMS_RB_CLASSIC,
		DEVICE_TYPE_DRUMS_RB,
		DEVICE_TYPE_DRUMS_GH,
		DEVICE_TYPE_KEYS_RB,
		DEVICE_TYPE_MIDI,
		DEVICE_TYPE_MOUSE,
		DEVICE_TYPE_KEYBOARD,
		DEVICE_TYPE_size
	};
	struct InputDevice;

	struct InputRawButton
	{
		InputRawButton() :
			button(-1),
			axis(-1),
			keybutton(-1),
			midi(-1)
		{ }
		InputRawButton(short button, char axis, int keybutton, short midi) :
			button(button),
			axis(axis),
			keybutton(keybutton),
			midi(midi)
		{ }

		short button;
		char axis;
		int keybutton;
		short midi;
		
		bool operator<(const InputRawButton &o) const { return false; }
		bool operator==(const InputRawButton &o) const
		{
			return button == o.button && axis == o.axis && keybutton == o.keybutton && midi == o.midi;
		}
	};

	/*
	--------------------------
	Input Binding items
	--------------------------
	*/

	//typedef std::map<InputRawButton, std::vector<InputGameButton>> InputBinding;
	typedef std::pair<InputRawButton, std::vector<InputGameButton>> InputBinder;
	typedef std::vector<InputBinder> InputBinding;

	void InputBindingsAddButton(InputBinding *bind, const InputRawButton &rawButton, InputGameButton gameButton);
	void InputBindingsRemoveButton(InputBinding *bind, const InputRawButton &rawButton, InputGameButton gameButton);

	struct InputAxisCalibration
	{
		int min;
		int max;
	};

	typedef int InputDeviceHandle;
	
	struct InputEventData
	{
		InputGameField m_buttonsPressed;	// Buttons pressed
		InputGameField m_buttonsReleased;	// Buttons released
		int m_heldButtons[INPUT_BUTTON_size];	// Keeps track of held buttons, and how many physical buttons are down

		/*
		Each pair contains the raw axis and calculated axis
		Both values will always represent the max status of the
		CALIBRATED state
		*/
		std::pair<int, float> axis[INPUT_BUTTON_size];

		InputDevice *device;

		double timestamp;
	};

	struct InputGLFW
	{
		int joyID;

		int buttonCount;
		const unsigned char *buttons;
		unsigned char *buttonsLast;

		int axisCount;
		const float *axis;
		float *axisLast;

		int povCount;
		unsigned char *povsLast;
	};

	struct InputDevice
	{
		InputDeviceHandle handle;
		WGE::Util::GUID guid;
		DeviceType type;									// Some implementations will determine which controller type

		InputBinding binding;								// Bindings for the device

		std::vector<InputAxisCalibration> axisCalibrations;	// Calibration data for all axis
		std::vector<bool> axisButtonControl;				// Keeps track of which axis is "pressed" for button conversion

		InputEventData m_lastInputEvent;

		InputGLFW glfwControllerData;
	};

	constexpr float INPUT_AXIS_ACTIVATION = 0.5f;

	constexpr InputDeviceHandle INPUT_HANDLE_INVALID = -1;
	constexpr InputDeviceHandle INPUT_HANDLE_KEYBOARD = 0;

	struct InputLostEventData
	{
		InputDeviceHandle lostHandle;
	};

	struct ContainerBinding
	{
		InputBinding binding;
		std::vector<InputAxisCalibration> calibrations;
	};
}

#endif

