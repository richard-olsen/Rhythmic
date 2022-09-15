#ifndef NOTE_H_
#define NOTE_H_

namespace Rhythmic
{
	enum NoteType
	{
		NOTE_1 = 0,
		NOTE_2 = 1,
		NOTE_3 = 2,
		NOTE_4 = 3,
		NOTE_5 = 4,
		NOTE_6 = 5,
		NOTE_OPEN = 6,

	};

	enum NoteFlag
	{
		NOTE_FLAG_STRUM = (1 << 0),
		NOTE_FLAG_HOPO = (1 << 1),
		NOTE_FLAG_TAP = (1 << 2),
		NOTE_FLAG_IS_CHORD = (1 << 3),
		NOTE_FLAG_IS_STARPOWER = (1 << 4),
		NOTE_FLAG_ADDING_STARPOWER = (1 << 5),
		NOTE_FLAG_CYMBAL = (1 << 6),
		NOTE_FLAG_HIT = (1 << 7),
		//NOTE_FLAG_BAR = (1 << 8) // The last note flag that is available
	};

	struct Note
	{
		float time;			// Notes time on the highway
		float sustain;		// The time on the highway when the sustain ends (will equal time if the note isn't a sustain)
		int chartPos;		// Note pos in chart units (used for sustain score)
		int chartLen;		// Sustain length in chart units. 0 if there isn't any sustain (used for sustain score)
		char chartTarget;	// To tell the ChartNote -> NotePool what the note is intended to be
		unsigned char note;			// Tells the game what kind of note this particular note is

		unsigned char flags; // Has a couple of flags that the game will use
		float lastScoreCheck;
		bool isBar;

		//bool isSustain;		// Determins if the note is a sustain note
		//bool isDouble;		// Tells the game that the note is double and should check notes behind or in front for the other(s)
		//NoteFlag flag;		// Tells the game whether this is a tap, hopo, or strum

		inline bool operator<(const Note &other)
		{
			if (time < other.time)
				return true;

			return note < other.note;
		}
	};
}
#define IS_NOTE(note,x) (note.flags & x)
#define SET_NOTE(note,x) (note.flags |= x)
#define CLEAR_NOTE(note,x) (note.flags &= ~(x))

#endif

