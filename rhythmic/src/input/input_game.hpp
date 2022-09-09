#ifndef INPUT_GAME_DATA_HPP_
#define INPUT_GAME_DATA_HPP_

#include <stdint.h>

namespace Rhythmic
{
	/*
	These are events that get sent along a different queue
	*/
	enum InputGameEvents
	{
		INPUT_EVENT_NULL = -1,
		INPUT_EVENT_DISCONNECT = -2,
		INPUT_EVENT_RESERVE_1 = -3,
		INPUT_EVENT_RESERVE_2 = -4,
		INPUT_EVENT_RESERVE_3 = -5,
		INPUT_EVENT_RESERVE_4 = -6,
	};

	/*
	These button types get sent along the queue
	*/
	enum InputGameButton
	{
		// All controllers
		INPUT_BUTTON_GREEN,
		INPUT_BUTTON_RED,
		INPUT_BUTTON_YELLOW,
		INPUT_BUTTON_BLUE,
		INPUT_BUTTON_ORANGE,

		INPUT_BUTTON_SELECT,
		INPUT_BUTTON_START,

		INPUT_BUTTON_DOWN,
		INPUT_BUTTON_UP,
		INPUT_BUTTON_LEFT,
		INPUT_BUTTON_RIGHT,

		// Guitar / Bass / Keys
		INPUT_BUTTON_WHAMMY,
		INPUT_BUTTON_STARPOWER,

		// Drum Specific
		INPUT_BUTTON_KICK,
		INPUT_BUTTON_KICK_ALT,
		INPUT_BUTTON_MOD_RB_DRUMS_PAD,
		INPUT_BUTTON_MOD_RB_DRUMS_CYMBAL,
		// Non RB drums
		INPUT_BUTTON_CYMBAL_YELLOW,
		INPUT_BUTTON_CYMBAL_BLUE,
		INPUT_BUTTON_CYMBAL_GREEN,

		//// RockBand 3 Keyboard
		//INPUT_BUTTON_KEY_LEFT,
		//INPUT_BUTTON_KEY_CENTER,
		//INPUT_BUTTON_KEY_RIGHT,
		//// All pro keys buttons
		//INPUT_BUTTON_KEY_C_1,
		//INPUT_BUTTON_KEY_CS_1,
		//INPUT_BUTTON_KEY_D_1,
		//INPUT_BUTTON_KEY_DS_1,
		//INPUT_BUTTON_KEY_E_1,
		//INPUT_BUTTON_KEY_F_1,
		//INPUT_BUTTON_KEY_FS_1,
		//INPUT_BUTTON_KEY_G_1,
		//INPUT_BUTTON_KEY_GS_1,
		//INPUT_BUTTON_KEY_A_1,
		//INPUT_BUTTON_KEY_AS_1,
		//INPUT_BUTTON_KEY_B_1,
		//INPUT_BUTTON_KEY_C_2,
		//INPUT_BUTTON_KEY_CS_2,
		//INPUT_BUTTON_KEY_D_2,
		//INPUT_BUTTON_KEY_DS_2,
		//INPUT_BUTTON_KEY_E_2,
		//INPUT_BUTTON_KEY_F_2,
		//INPUT_BUTTON_KEY_FS_2,
		//INPUT_BUTTON_KEY_G_2,
		//INPUT_BUTTON_KEY_GS_2,
		//INPUT_BUTTON_KEY_A_2,
		//INPUT_BUTTON_KEY_AS_2,
		//INPUT_BUTTON_KEY_B_2,
		//INPUT_BUTTON_KEY_C_3,

		//INPUT_BUTTON_MOD_3,
		//INPUT_BUTTON_MOD_4,
		//INPUT_BUTTON_MOD_5,
		//INPUT_BUTTON_MOD_6,

		INPUT_BUTTON_size		// Not a button DO NOT USE
	};

	typedef uint64_t InputGameField;

	const char *InputGetButtonName(InputGameButton button);
}

#define ButtonToFlag(button) ((InputGameField)((InputGameField)1 << (InputGameField)(button)))

#endif
