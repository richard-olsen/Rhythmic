#include "input_game.hpp"

namespace Rhythmic
{
	static const char *g_inputButtonNames[INPUT_BUTTON_size] = {
		/*
		All instruments have these buttons (with the exception of Pro Keys)
		*/
		"Green/B1",			// Accept
		"Red/B2",				// Back
		"Yellow/B3",			// Sort
		"Blue/W1",				// Search
		"Orange/W2",			// Change Category
		"White3",			// 6 fret

		"Select",
		"Start",

		"Down",				// This counts as strumming for guitar/bass
		"Up",					// This counts as strumming for guitar/bass
		"Left",
		"Right",

		/*
		Guitar/Bass/Keys specific
		*/
		"Whammy",
		"Starpower",


		/*
		Specific to Rock Band style drums
		*/
		"Kick Primary",
		"Kick Secondary",
		"Pad Modifier",				// Tells the game the input was a pad
		"Cymbal Modifier",		// Tells the game the input was a cymbal
		/*
		For non RB drums
		*/
		"Cymbal Yellow",
		"Cymbal Blue",
		"Cymbal Green",


		/*
		Pro Keys specific
		*/

		/*"Left Side",
		"Center",
		"Right Side",

		"C  1",
		"C# 1",
		"D  1",
		"D# 1",
		"E  1",
		"F  1",
		"F# 1",
		"G  1",
		"G# 1",
		"A  1",
		"A# 1",
		"B  1",
		"C  2",
		"C# 2",
		"D  2",
		"D# 2",
		"E  2",
		"F  2",
		"F# 2",
		"G  2",
		"G# 2",
		"A  2",
		"A# 2",
		"B  2",
		"C  3",*/
	};

	const char *InputGetButtonName(InputGameButton button)
	{
		return g_inputButtonNames[(int)button];
	}
}