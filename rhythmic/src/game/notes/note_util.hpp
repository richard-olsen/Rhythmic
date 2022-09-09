#ifndef RHYTHMIC_NOTE_UTILITIES_HPP_
#define RHYTHMIC_NOTE_UTILITIES_HPP_

#include "note.hpp"
#include <vector>

namespace Rhythmic
{
	typedef std::vector<Note> NotePool;


	/*
	Retrieves the end index for chord notes
	*/
	unsigned int GetChordEndingIndex(NotePool &notePool, unsigned int index);

	/*
	Generates an int of notes that should
	be hit

	If chordSize is -1, or MAX of unsigned int,
	the function will find the chordSize itself
	*/
	unsigned int GetNotesToHit(NotePool &notePool, unsigned int index, unsigned int chordSize = -1);
}

#endif

