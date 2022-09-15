#include <random>
#include <vector>

#include "../chart/chart.hpp"

#include "notes/note.hpp"
#include "engine_vals.hpp"
#include "notes/note_util.hpp"
#include "instrument/instrument.hpp"

#include <wge/core/logging.hpp>

#include "modifiers.hpp"

namespace Rhythmic 
{

	void ApplyModifiers(Instrument *instrument, int modifiers, bool is4Lane) {
		NotePool *notePool = instrument->GetNotePoolTemplate();
		if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_GUITAR || instrument->GetInstrumentType() == INSTRUMENT_TYPE_BASS || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRET || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRETBASS) {
			if (IS_MODIFIER(modifiers, ALL_STRUMS)) ConvertAllToType(*notePool, NOTE_FLAG_STRUM);
			if (IS_MODIFIER(modifiers, ALL_HOPOS)) ConvertAllToType(*notePool, NOTE_FLAG_HOPO);
			if (IS_MODIFIER(modifiers, ALL_TAPS)) ConvertAllToType(*notePool, NOTE_FLAG_TAP);
			if (IS_MODIFIER(modifiers, ALL_OPENS)) ConvertAllToType(*notePool, NOTE_OPEN);

			if (IS_MODIFIER(modifiers, HOPOS_TO_TAPS)) HOPOsToTaps(*notePool);
		} else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_DRUMS) {
			if (IS_MODIFIER(modifiers, ALL_CYMBALS)) AllToCymbals(*notePool, instrument);
			if (IS_MODIFIER(modifiers, SHUFFLE_CYMBALS)) ShuffleCymbals(*notePool, instrument);
		}

		// These are here because they can be both drum and guitar
		if (IS_MODIFIER(modifiers, NOTE_SHUFFLE)) ShuffleNotes(*notePool, instrument, is4Lane);
		if (IS_MODIFIER(modifiers, MIRROR_MODE)) MirrorNotes(*notePool, instrument, is4Lane);
		if (IS_MODIFIER(modifiers, DOUBLE_NOTES)) DoubleNotes(*notePool, instrument, is4Lane);
	}

	void ConvertAllToType(NotePool &notePool, int flag) {
		for (int i = 0; i < notePool.size(); i++) {
			Note &note = notePool[i];

			switch (flag) {
			case NOTE_FLAG_STRUM:
				if (IS_NOTE(note, NOTE_FLAG_HOPO)) CLEAR_NOTE(note, NOTE_FLAG_HOPO);
				if (IS_NOTE(note, NOTE_FLAG_TAP)) CLEAR_NOTE(note, NOTE_FLAG_TAP);

				SET_NOTE(note, NOTE_FLAG_STRUM);
				break;
			case NOTE_FLAG_HOPO:
				if (IS_NOTE(note, NOTE_FLAG_STRUM)) CLEAR_NOTE(note, NOTE_FLAG_STRUM);
				if (IS_NOTE(note, NOTE_FLAG_TAP)) CLEAR_NOTE(note, NOTE_FLAG_TAP);

				SET_NOTE(note, NOTE_FLAG_HOPO);
				break;
			case NOTE_FLAG_TAP:
				if (IS_NOTE(note, NOTE_FLAG_HOPO)) CLEAR_NOTE(note, NOTE_FLAG_HOPO);
				if (IS_NOTE(note, NOTE_FLAG_STRUM)) CLEAR_NOTE(note, NOTE_FLAG_STRUM);

				SET_NOTE(note, NOTE_FLAG_TAP);
				break;
			case NOTE_OPEN:
				int chordEnding = GetChordEndingIndex(notePool, i);
				if (chordEnding > 1) {
					for (int j = 1; j < chordEnding; j++) {
						notePool.erase(std::next(notePool.begin(), i));
					}
				}
				note.note = NOTE_OPEN;
			}
		}
	}

	void AllToCymbals(NotePool &notePool, Instrument *instrument) {
		if (!instrument->IsPro()) return;

		for (Note &note : notePool) {
			if (IS_NOTE(note, NOTE_FLAG_CYMBAL) || note.note == NOTE_OPEN) continue;

			SET_NOTE(note, NOTE_FLAG_CYMBAL);

			if (note.note == NOTE_1) note.note = NOTE_2;
		}
	}

	void ShuffleCymbals(NotePool &notePool, Instrument *instrument) {
		if (!instrument->IsPro()) return;

		std::random_device device;
		std::mt19937 engine(device());
		std::uniform_int_distribution<int> dist(0, 2);

		for (Note &note : notePool) {
			if (note.note == NOTE_OPEN) continue;

			bool isShuffle = false;

			if (dist(engine) == 1) isShuffle = true;

			if (isShuffle) {
				if (IS_NOTE(note, NOTE_FLAG_CYMBAL)) {
					CLEAR_NOTE(note, NOTE_FLAG_CYMBAL);
				}
				else {
					if (note.note == NOTE_1) note.note = NOTE_2;

					note.flags |= NOTE_FLAG_CYMBAL;
				}
			}

		}
	}

	void HOPOsToTaps(NotePool &notePool) {
		for (Note &note : notePool) {
			if (IS_NOTE(note, NOTE_FLAG_HOPO)) {
				CLEAR_NOTE(note, NOTE_FLAG_HOPO);
				SET_NOTE(note, NOTE_FLAG_TAP);
			}
		}
	}

	void ShuffleNotes(NotePool &notePool, Instrument *instrument, bool is4Lane) {
		if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_GUITAR || instrument->GetInstrumentType() == INSTRUMENT_TYPE_BASS) {
			std::random_device device;
			std::mt19937 engine(device());
			std::uniform_int_distribution<int> dist(0, 4);

			unsigned int i = 0;
			while (i < notePool.size())
			{
				int chordEnding = GetChordEndingIndex(notePool, i);

				int shuffle = 0; // Bitfield

				for (int index = 0; index < chordEnding; index++) {
					Note &note = notePool[i + index];

					if (note.note == NOTE_OPEN) continue;

					do {
						auto const random = dist(engine);
						note.note = random;
					} while (shuffle & (1 << note.note));

					shuffle |= (1 << note.note);
				}

				i += chordEnding;
			}
		}
		else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_DRUMS) {
			if (is4Lane) {
				std::random_device device;
				std::mt19937 engine(device());

				unsigned int i = 0;
				while (i < notePool.size())
				{
					int chordEnding = GetChordEndingIndex(notePool, i);
					int min = 0;

					int shuffle = 0; // Bitfield

					for (int index = 0; index < chordEnding; index++) {
						Note &note = notePool[i + index];

						if (note.note != NOTE_OPEN) {
							if (IS_NOTE(note, NOTE_FLAG_CYMBAL)) min = 1;

							std::uniform_int_distribution<int> dist(min, 3);

							do {
								auto const random = dist(engine);
								note.note = random;
							} while (shuffle & (1 << note.note));

							shuffle |= (1 << note.note);
						}
					}

					i += chordEnding;
				}
			}
			else {
				// 5 Lane
			}
		}
		else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRET || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRETBASS) {
			std::random_device device;
			std::mt19937 engine(device());
			std::uniform_int_distribution<int> dist(0, 5);

			unsigned int i = 0;
			while (i < notePool.size())
			{
				int chordEnding = GetChordEndingIndex(notePool, i);

				int shuffle = 0; // Bitfield

				bool bar1 = false, bar2 = false, bar3 = false;
				int barone = 0, bartwo = 0, barthree = 0;


				for (int index = 0; index < chordEnding; index++) {
					Note& note = notePool[i + index];
					note.isBar = false;

					if (note.note == NOTE_OPEN) continue;

					do {
						auto const random = dist(engine);
						note.note = random;

					} while (shuffle & (1 << note.note));

					if (bar1 && (note.note == NOTE_1 || note.note == NOTE_4)) {
						note.isBar = true;
						Note& temp = notePool[i + barone];
						temp.isBar = true;
					}
					else if (bar2 && (note.note == NOTE_2 || note.note == NOTE_5)) {
						note.isBar = true;
						Note& temp = notePool[i + bartwo];
						temp.isBar = true;
					}
					else if (bar3 && (note.note == NOTE_3 || note.note == NOTE_6)) {
						note.isBar = true;
						Note& temp = notePool[i + barthree];
						temp.isBar = true;
					}

					if (note.note == NOTE_1 || note.note == NOTE_4) {
						bar1 = true;
						barone = index;
					}
					else if (note.note == NOTE_2 || note.note == NOTE_5) {
						bar2 = true;
						bartwo = index;
					}
					else if (note.note == NOTE_3 || note.note == NOTE_6) {
						bar3 = true;
						barthree = index;
					}

					shuffle |= (1 << note.note);
				}

				i += chordEnding;
			}
		}
		
	}

	void MirrorNotes(NotePool &notePool, Instrument *instrument, bool is4Lane) {
		if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_GUITAR || instrument->GetInstrumentType() == INSTRUMENT_TYPE_BASS) {
			for (Note &note : notePool) {
				switch (note.note) {
				case NOTE_1:
					note.note = NOTE_5;
					break;
				case NOTE_2:
					note.note = NOTE_4;
					break;
				case NOTE_4:
					note.note = NOTE_2;
					break;
				case NOTE_5:
					note.note = NOTE_1;
					break;
				}
			}
		}
		else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_DRUMS) {
			if (is4Lane) {
				for (Note &note : notePool) {
					if (note.note == NOTE_OPEN) continue;

					switch (note.note) {
					case NOTE_1:
						note.note = NOTE_4;
						break;
					case NOTE_2:
						note.note = NOTE_3;
						break;
					case NOTE_3:
						note.note = NOTE_2;
						break;
					case NOTE_4:
						if (IS_NOTE(note, NOTE_FLAG_CYMBAL)) {
							note.note = NOTE_2;
						}
						else note.note = NOTE_1;
						break;
					}
				}
			}
			else {
				// 5 Lane
			}
			
		}
		else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRET || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRETBASS) {
			for (Note& note : notePool) {
				switch (note.note) {
				case NOTE_1:
					note.note = NOTE_6;
					break;
				case NOTE_2:
					note.note = NOTE_5;
					break;
				case NOTE_3:
					note.note = NOTE_4;
					break;
				case NOTE_4:
					note.note = NOTE_3;
					break;
				case NOTE_5:
					note.note = NOTE_2;
					break;
				case NOTE_6:
					note.note = NOTE_1;
					break;
				}
			}
		}

	}

	void DoubleNotes(NotePool &notePool, Instrument *instrument, bool is4Lane) {
		if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_GUITAR || instrument->GetInstrumentType() == INSTRUMENT_TYPE_BASS) {
			unsigned int i = 0;
			while (i < notePool.size())
			{
				int chordEnding = GetChordEndingIndex(notePool, i);

				if (chordEnding == 1) { // Single note
					Note doubleNote = notePool[i];

					if (notePool[i].note != NOTE_OPEN)
					{
						switch (doubleNote.note)
						{
						case NOTE_1:
							doubleNote.note = NOTE_2;
							break;
						case NOTE_2:
							doubleNote.note = NOTE_3;
							break;
						case NOTE_3:
							doubleNote.note = NOTE_4;
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5;
							break;
						case NOTE_5:
							doubleNote.note = NOTE_3;
							break;
						}

						notePool[i].flags |= NOTE_FLAG_IS_CHORD;
						doubleNote.flags |= NOTE_FLAG_IS_CHORD;

						notePool.emplace(std::next(notePool.begin(), i), doubleNote);

						i += chordEnding + 1;
					}
					else i++;
				}
				else if (chordEnding == 2) {
					Note start = notePool[i];
					Note end = notePool[i + chordEnding - 1];

					Note doubleNote = notePool[i];

					doubleNote.flags |= NOTE_FLAG_IS_CHORD;

					switch (start.note) {
					case NOTE_1:
						switch (end.note) {
						case NOTE_2:
							doubleNote.note = NOTE_3; // GRY
							break;
						case NOTE_3:
							doubleNote.note = NOTE_4; // GYB
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5; // GBO
							break;
						case NOTE_5:
							doubleNote.note = NOTE_3; // GYO
							break;
						}
					case NOTE_2:
						switch (end.note) {
						case NOTE_3:
							doubleNote.note = NOTE_4; // RYB
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5; // RYO
							break;
						case NOTE_5:
							doubleNote.note = NOTE_4; // RBO
							break;
						}
						break;
					case NOTE_3:
						switch (end.note) {
						case NOTE_4:
							doubleNote.note = NOTE_5; // YBO
							break;
						case NOTE_5:
							doubleNote.note = NOTE_2; // RYO
							break;
						}
						break;
					case NOTE_4:
						switch (end.note) {
						case NOTE_5:
							doubleNote.note = NOTE_1; // GBO
							break;
						}
						break;
					}

					notePool.emplace(std::next(notePool.begin(), i), doubleNote);
					i += chordEnding + 1;
				}
				else
					i += chordEnding;
			}
		}
		else if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_DRUMS) { // SOMETIMES CREATES 3 PAD NOTES
			std::random_device device;
			std::mt19937 engine(device());
			std::uniform_int_distribution<int> dist(1, 3);

			if (is4Lane) {
				unsigned int i = 0;

				while (i < notePool.size()) {
					int chordEnding = GetChordEndingIndex(notePool, i);

					Note note = notePool[i];

					if (chordEnding == 1) {
						if (note.note != NOTE_OPEN) {

							Note doubleNote = note;

							switch (note.note) {
							case NOTE_1:
								doubleNote.note = NOTE_2;
								break;
							case NOTE_2:
								doubleNote.note = NOTE_3;
								break;
							case NOTE_3:
								doubleNote.note = NOTE_4;
								break;
							case NOTE_4:
								if (IS_NOTE(doubleNote, NOTE_FLAG_CYMBAL)) doubleNote.note = NOTE_2;
								else doubleNote.note = NOTE_1;
								break;
							}
							notePool.emplace(std::next(notePool.begin(), i), doubleNote);
							chordEnding++;
							goto END_OF_LOOP;
						}
					}
					else if (chordEnding == 2) {
						Note start = notePool[i];
						Note end = notePool[i + 1];

						Note switchNote = start;

						if (start.note != NOTE_OPEN && end.note != NOTE_OPEN) {
							goto END_OF_LOOP;
						}

						if (start.note == NOTE_OPEN) {
							Note switchNote = end;
						}

						Note doubleNote = switchNote;

						switch (switchNote.note) {
						case NOTE_1:
							doubleNote.note = NOTE_2;
							break;
						case NOTE_2:
							doubleNote.note = NOTE_3;
							break;
						case NOTE_3:
							doubleNote.note = NOTE_4;
							break;
						case NOTE_4:
							if(IS_NOTE(doubleNote, NOTE_FLAG_CYMBAL)) doubleNote.note = NOTE_2;
							else doubleNote.note = NOTE_1;
							break;
						}
						notePool.emplace(std::next(notePool.begin(), i), doubleNote);
						chordEnding++;
					}
				END_OF_LOOP:
					i += chordEnding;
				}
			}
			else {
				// 5 Lane
			}
		}
		if (instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRET || instrument->GetInstrumentType() == INSTRUMENT_TYPE_6FRETBASS) {
			unsigned int i = 0;
			while (i < notePool.size())
			{
				int chordEnding = GetChordEndingIndex(notePool, i);

				if (chordEnding == 1) { // Single note
					Note doubleNote = notePool[i];

					if (notePool[i].note != NOTE_OPEN)
					{
						switch (doubleNote.note)
						{
						case NOTE_1:
							doubleNote.note = NOTE_2;
							break;
						case NOTE_2:
							doubleNote.note = NOTE_3;
							break;
						case NOTE_3:
							doubleNote.note = NOTE_4;
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5;
							break;
						case NOTE_5:
							doubleNote.note = NOTE_6;
							break;
						case NOTE_6:
							doubleNote.note = NOTE_4;
							break;
						}

						notePool[i].flags |= NOTE_FLAG_IS_CHORD;
						doubleNote.flags |= NOTE_FLAG_IS_CHORD;

						notePool.emplace(std::next(notePool.begin(), i), doubleNote);

						i += chordEnding + 1;
					}
					else i++;
				}
				else if (chordEnding == 2) {
					Note start = notePool[i];
					Note end = notePool[i + chordEnding - 1];

					Note doubleNote = notePool[i];
					if (doubleNote.isBar) doubleNote.isBar = false;

					doubleNote.flags |= NOTE_FLAG_IS_CHORD;

					switch (start.note) {
					case NOTE_1:
						switch (end.note) {
						case NOTE_2:
							doubleNote.note = NOTE_3; // 
							break;
						case NOTE_3:
							doubleNote.note = NOTE_4; // 
							doubleNote.isBar = true;
							start.isBar = true;
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5; // 
							break;
						case NOTE_5:
							doubleNote.note = NOTE_6; // 
							break;
						case NOTE_6:
							doubleNote.note = NOTE_4; // 
							doubleNote.isBar = true;
							start.isBar = true;
							break;
						}
					case NOTE_2:
						switch (end.note) {
						case NOTE_3:
							doubleNote.note = NOTE_4; // 
							break;
						case NOTE_4:
							doubleNote.note = NOTE_5; // 
							doubleNote.isBar = true;
							start.isBar = true;
							break;
						case NOTE_5:
							doubleNote.note = NOTE_6; // 
							break;
						case NOTE_6:
							doubleNote.note = NOTE_5; // 
							doubleNote.isBar = true;
							start.isBar = true;
							break;
						}
						break;
					case NOTE_3:
						switch (end.note) {
						case NOTE_4:
							doubleNote.note = NOTE_5; // 
							break;
						case NOTE_5:
							doubleNote.note = NOTE_6; // 
							doubleNote.isBar = true;
							start.isBar = true;
							break;
						case NOTE_6:
							doubleNote.note = NOTE_4; // 
							break;
						}
						break;
					case NOTE_4:
						switch (end.note) {
						case NOTE_5:
							doubleNote.note = NOTE_6; // 
							break;
						case NOTE_6:
							doubleNote.note = NOTE_2; // 
							break;
						}
						break;
					case NOTE_5:
						switch (end.note) {
						case NOTE_6:
							doubleNote.note = NOTE_1; // 
							break;
						}
						break;
					}
					

					notePool.emplace(std::next(notePool.begin(), i), doubleNote);
					i += chordEnding + 1;
				}
				else
					i += chordEnding;
			}
		}
	}

}