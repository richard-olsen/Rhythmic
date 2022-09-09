#ifndef MODIFIERS_NEW_H
#define MODIFIERS_NEW_H

#include "../game/notes/note.hpp"

#include <vector>

namespace Rhythmic 
{

#define IS_MODIFIER(modifiers, x) (modifiers & x)
#define SET_MODIFIER(modifiers, x) (modifiers |= x)
#define CLEAR_MODIFIER(modifiers, x) (modifiers &= ~(x))

	enum Modifiers {
		ALL_STRUMS = (1 << 0),		// Guitar only
		ALL_HOPOS = (1 << 1),		// Guitar only
		ALL_TAPS = (1 << 2),		// Guitar only
		ALL_OPENS = (1 << 3),		// Guitar only
		HOPOS_TO_TAPS = (1 << 4),   // Guitar only
		NOTE_SHUFFLE = (1 << 5),	// Guitar + Drums
		MIRROR_MODE = (1 << 6),		// Guitar + Drums
		DOUBLE_NOTES = (1 << 7),	// Guitar + Drums

		ALL_CYMBALS = (1 << 8),		// Pro Drums only
		SHUFFLE_CYMBALS = (1 << 9)  // Pro Drums only
	};

	void ApplyModifiers(Instrument *instrument, int modifiers, bool is4Lane);

	void ConvertAllToType(NotePool &notePool, int flag);
	void AllToCymbals(NotePool &notePool, Instrument *instrument);
	void ShuffleCymbals(NotePool &notePool, Instrument *instrument);
	void HOPOsToTaps(NotePool &notePool);

	void ShuffleNotes(NotePool &notePool, Instrument *instrument, bool is4Lane);
	void MirrorNotes(NotePool &notePool, Instrument *instrument, bool is4Lane);
	void DoubleNotes(NotePool &notePool, Instrument *instrument, bool is4Lane);

}
#endif