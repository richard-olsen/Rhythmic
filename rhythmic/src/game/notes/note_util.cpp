#include "note_util.hpp"

namespace Rhythmic
{
	unsigned int GetChordEndingIndex(NotePool &notePool, unsigned int index)
	{
		size_t notePoolSize = notePool.size();

		if (index >= notePoolSize)
			return 0;
		if (IS_NOTE(notePool[index], NOTE_FLAG_IS_CHORD)) // The idea of this being the first note in the doubles, and extending the noteAmount to the others
		{
			unsigned int noteAmount = 0;

			while ((index + noteAmount) < notePoolSize &&
				notePool[index + noteAmount].time == notePool[index].time)
			{
				noteAmount++;
			}

			return noteAmount;
		}

		return 1; // There is only one note if it ain't a double
	}

	unsigned int GetNotesToHit(NotePool &notePool, unsigned int index, unsigned int chordSize)
	{
		if (chordSize == -1)
			chordSize = GetChordEndingIndex(notePool, index);

		unsigned int notesToHit = 0;

		for (unsigned int i = 0; i < chordSize; i++)
		{
			notesToHit |= (1 << notePool[index + i].note);
		}

		return notesToHit;
	}
}