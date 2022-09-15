#include "midi_parser.hpp"

#include "../util/midifile/MidiFile.h"
#include "../util/misc.hpp"

#include <wge/core/logging.hpp>

#include "../game/notes/note.hpp"

#include <glm/common.hpp>

namespace Rhythmic
{
	void LoadEvents(const smf::MidiEventList &list, Chart &chart);
	void LoadNotes(const smf::MidiEventList &list, Chart &chart, int instrument);
	
	int NoteNumberToDifficulty(int noteNumber);
	int NoteNumberToPadDrums(int noteNumber);
	int NoteNumberToPad(int noteNumber);
	int NoteNumberToPadSixFret(int noteNumber);


	bool LoadChartFromMidi(Chart *chart, const std::string &chart_file_name, bool loadHeaderOnly)
	{
		smf::MidiFile mid = smf::MidiFile();
		bool success = mid.read(chart_file_name);
		if (!success)
			return false;

		mid.doTimeAnalysis();
		mid.linkNotePairs();

		chart->songData.resolution = mid.getTicksPerQuarterNote();
		chart->songData.offset = 0;
		
		// Add time signatures and bpms first

		int midEvntDefCount = mid.getEventCount(0);
		for (unsigned int i = 0; i < midEvntDefCount; i++)
		{
			const smf::MidiEvent &midEvntDef = mid.getEvent(0, i);

			if (midEvntDef.isTimeSignature())
			{
				ChartTS ts;
				ts.position = midEvntDef.tick;
				ts.numerator = midEvntDef[3];
				ts.denominator = std::pow(2, midEvntDef[4]);
				chart->listTS.push_back(ts);
			}
			else if (midEvntDef.isTempo())
			{
				ChartBPM bpm;
				bpm.position = midEvntDef.tick;
				bpm.bpm = (unsigned int)(midEvntDef.getTempoBPM() * 1000.0);
				chart->listBPM.push_back(bpm);
			}
		}

		if (loadHeaderOnly)
			return false;

		// Iterate through the tracks and look for key events for notes, sp phrases, section names, etc.
		for (unsigned int track = 1; track < mid.getTrackCount(); track++)
		{
			const smf::MidiEventList &midEventList = mid[track];
			smf::MidiEvent name = midEventList[0];
			
			if (!name.isMetaMessage())
				continue;

			std::string type = name.getMetaContent();
			if (Util::compareStringCaseInsensitive(type, "events") == 0)
			{
				LoadEvents(midEventList, *chart);
			}
			else if (Util::compareStringCaseInsensitive(type, "part guitar") == 0)
			{
				LoadNotes(midEventList, *chart, INSTRUMENT_TYPE_GUITAR);
			}
			else if (Util::compareStringCaseInsensitive(type, "part bass") == 0)
			{
				LoadNotes(midEventList, *chart, INSTRUMENT_TYPE_BASS);
			}
			else if (Util::compareStringCaseInsensitive(type, "part drums") == 0)
			{
				LoadNotes(mid[track], *chart, INSTRUMENT_TYPE_DRUMS);
			}
			else if (Util::compareStringCaseInsensitive(type, "part guitar ghl") == 0)
			{
				LoadNotes(mid[track], *chart, INSTRUMENT_TYPE_6FRET);
			}

			else if (Util::compareStringCaseInsensitive(type, "part bass ghl") == 0)
			{
				LoadNotes(mid[track], *chart, INSTRUMENT_TYPE_6FRETBASS);
			}
		}

		if (chart->listTS.empty())
		{
			ChartTS ts;
			ts.numerator = 4;
			ts.denominator = 4;
			ts.position = 0;
			ts.time = 0;
			chart->listTS.push_back(ts);
		}

		if (chart->listBPM.empty()) {
			ChartBPM bpm;
			bpm.bpm = 120;
			bpm.position = 0;
			bpm.time = 0;
			chart->listBPM.push_back(bpm);
		}


		mid.clear();
		return true;
	}
	void LoadEvents(const smf::MidiEventList &list, Chart &chart)
	{
		for (int i = 0; i < list.getEventCount(); i++)
		{
			smf::MidiEvent midEvent = list[i];

			if (midEvent.isMetaMessage())
			{
				std::string midText = midEvent.getMetaContent();
				
				if (midText.substr(0, 9).compare("[section ") == 0)
				{
					ChartSection section;
					section.position = midEvent.tick;
					section.name = midText.substr(9, midText.size() - 10);
					chart.listSection.push_back(section);
				}
			}
		}
	}
	void LoadNotes(const smf::MidiEventList &list, Chart &chart, int instrument)
	{
		int sustainCorrection = (int)(64.0f * chart.songData.resolution / 192.0f);
		chart.supports[instrument] = true;

		std::vector<smf::MidiEvent const *> sysexEvents;
		std::vector<smf::MidiEvent const *> forceEvents;
		std::vector<smf::MidiEvent const *> cymbalEvents;

		for (int i = 0; i < list.getEventCount(); i++)
		{
		 	const smf::MidiEvent &midEvent = list[i];

		 	if (midEvent.isMetaMessage()) // Useless, unless we are getting events (sections/solos)
		 	{
		 		continue; // Just ignore it for now, until sections loading becomes a thing
		 	}
		 	else if (midEvent.isNoteOn())
		 	{
				int sustain = midEvent.getLinkedEvent()->tick - midEvent.tick;

		 		int noteNumber = midEvent.getKeyNumber();

		 		if (noteNumber == 116) // Starpower Phrase
		 		{
					ChartStarpower sp;
					sp.position = midEvent.tick;
					sp.length = sustain;
					sp.type = 2;
					for (int diff = 0; diff < 4; diff++)
						chart.spPhrases[instrument][diff].push_back(sp);
		 			continue;
		 		}

		 		// Determine difficulty
		 		int difficulty = NoteNumberToDifficulty(noteNumber);
				if (difficulty < 0) // It could possibly be cymbal notes
				{
					// (looking at it from the drums perspective, not the games)
					if (instrument == INSTRUMENT_TYPE_DRUMS && // Only work if it's drums
					   (noteNumber == 110 || // Yellow pad to cymbal
						noteNumber == 111 || // Blue pad to cymbal
						noteNumber == 112))  // Green pad to cymbal
						cymbalEvents.push_back(&midEvent);
					continue;	
				}


		 		ChartNote cNote;
		 		cNote.position = midEvent.tick;
				cNote.sustain = 0;
				cNote.tryForce = 0;

		 		// Checks to see if it's a force note
		 		if (instrument != INSTRUMENT_TYPE_DRUMS) // Change to instrument != drums
		 		{
		 			switch (noteNumber)
		 			{
		 				case 65:
		 				case 66:		
		 				case 77:
		 				case 78:
		 				case 89:
		 				case 90:
		 				case 101:
		 				case 102:
		 					//forceNotesList.Add(note);       // Store the event for later processing and continue
							forceEvents.push_back(&midEvent);
		 					continue;
		 				default:
		 					break;
		 			}
		 		}

		 		// Get the right note

				if (sustain <= sustainCorrection)
					sustain = 0;
		 		cNote.sustain = sustain;


				//if (instrument == INSTRUMENT_TYPE_GUITAR && difficulty == NOTES_EXPERT)
				//	int breakpoint = 5;

				if (instrument == INSTRUMENT_TYPE_DRUMS)
				{
					cNote.note = NoteNumberToPadDrums(noteNumber);
					if (cNote.note == 2 || cNote.note == 3 || cNote.note == 4)
						cNote.tryForce |= CHART_NOTE_TRY_CYMBAL;
				}
				else if (instrument == INSTRUMENT_TYPE_6FRET || instrument == INSTRUMENT_TYPE_6FRETBASS) {
					cNote.note = NoteNumberToPadSixFret(noteNumber);
				}
				else
			 		cNote.note = NoteNumberToPad(noteNumber);

				if (chart.lastNotePosWithSustain < cNote.position + cNote.sustain)
					chart.lastNotePosWithSustain = cNote.position + cNote.sustain;

		 		chart.notes[instrument][difficulty].push_back(cNote);
		 	}
			else if ((midEvent.data()[0] & 0xF0) == 0xF0) // It's a sysex event
			{
				sysexEvents.push_back(&midEvent);
			}
		}

		// Cymbals have a range. Any note within the range gets forced to be a cymbal note
		for (int i = 0; i < cymbalEvents.size(); i++)
		{
			// Color of RockBand drums
			// Red | Yellow | Blue | Green
			// 110 - Yellow Pad -> Cymbal
			// 111 - Blue Pad -> Cymbal
			// 112 - Green Pad -> Cymbal

			smf::MidiEvent const *midEvent = cymbalEvents[i];

			int note = midEvent->getKeyNumber();

			int end = midEvent->getLinkedEvent()->tick;

			int targetNote = 0;
			if (note == 110) targetNote = 2;
			if (note == 111) targetNote = 3;
			if (note == 112) targetNote = 4;

			for (int j = 0; j < 4; j++)
			{
				std::vector<ChartNote> &notes = chart.notes[instrument][j];
				if (notes.empty())
					continue;

				int startIt = 0;
				while (notes[startIt].position < midEvent->tick)
					startIt++;

				for (; startIt < notes.size() &&
						notes[startIt].position < end &&
						notes[startIt].position >= midEvent->tick; startIt++)
					if (notes[startIt].note == targetNote)
						notes[startIt].tryForce &= ~CHART_NOTE_TRY_CYMBAL;
			}
		}

		// These are achieved with using a range. Notes within the tap range get turned into a tap. Open acts like a noteOn and noteOff
		// but open notes aren't connected
		for (int i = 0; i < sysexEvents.size(); i++)
		{
			smf::MidiEvent const *midEvent = sysexEvents[i];
			// Tap Ranges
			if (midEvent->size() == 9 && (*midEvent)[5] == 255 && (*midEvent)[7] == 1)
			{
				int end = 0;

				for (int j = i + 1; j < sysexEvents.size(); j++)
				{
					smf::MidiEvent const *tapOff = sysexEvents[j];

					if (tapOff->size() == 9 && (*tapOff)[5] == (*midEvent)[5] && (*tapOff)[7] == 0)
					{
						end = tapOff->tick;
						break;
					}
				}

				for (int difficulty = 0; difficulty < 5; difficulty++)
				{
					std::vector<ChartNote> &notes = chart.notes[instrument][difficulty];
					if (notes.empty())
						continue;

					int startIt = 0;

					while (notes[startIt].position < midEvent->tick)
						startIt++;

					for (; startIt < notes.size() &&
						notes[startIt].position < end &&
						notes[startIt].position >= midEvent->tick; startIt++)
						notes[startIt].tryForce |= CHART_NOTE_TRY_TAP;
				}
			}

			// Open
			else if (midEvent->size() == 9 && (*midEvent)[5] >= 0 && (*midEvent)[5] < 4 && (*midEvent)[7] == 1)
			{
				int difficulty = -1;

				if ((*midEvent)[5] == 0) difficulty = NOTES_EASY;
				if ((*midEvent)[5] == 1) difficulty = NOTES_MEDIUM;
				if ((*midEvent)[5] == 2) difficulty = NOTES_HARD;
				if ((*midEvent)[5] == 3) difficulty = NOTES_EXPERT;
				if (difficulty < 0 || difficulty > 3)
					continue;
				
				int end = 0;

				for (int j = i + 1; j < sysexEvents.size(); j++)
				{
					smf::MidiEvent const *openOff = sysexEvents[j];

					if (openOff->size() == 9 && (*openOff)[5] == (*midEvent)[5] && (*openOff)[7] == 0)
					{
						end = openOff->tick;
						break;
					}
				}

				std::vector<ChartNote> &notes = chart.notes[instrument][difficulty];

				if (notes.empty())
				{
					int startIt = 0;
					while (notes[startIt].position < midEvent->tick)
						startIt++;

					for (; startIt < notes.size() &&
						notes[startIt].position < end &&
						notes[startIt].position >= midEvent->tick; startIt++)
						notes[startIt].note = 7;
				}
			}

			// else
			// {
			// 	smf::MidiEvent const *midEvent = sysexEvents[i];

			// 	int tick = midEvent->tick;
			// }
			
		}

		for (unsigned int i = 0; i < forceEvents.size(); i++)
		{
			smf::MidiEvent const *midEvent = forceEvents[i];
			int endingForcePos = midEvent->getLinkedEvent()->tick;

			bool forceHopo = midEvent->getKeyNumber() % 2 != 0; // Force HOPO if the 

			int difficulty = NoteNumberToDifficulty(midEvent->getKeyNumber());
			if (difficulty < 0 || difficulty > 3)
				continue;

			std::vector<ChartNote> &notes = chart.notes[instrument][difficulty];
			if (notes.empty())
				continue;

			int startIt = 0;
			while (notes[startIt].position < midEvent->tick)
				startIt++;

			//while (chart.notes[instrument][difficulty][startIt].position < endingForcePos)
			for (; startIt < notes.size() &&
					notes[startIt].position < endingForcePos &&
					notes[startIt].position >= midEvent->tick; startIt++)
			{
				// Force notes
				notes[startIt].tryForce |= forceHopo ? CHART_NOTE_TRY_HOPO : CHART_NOTE_TRY_STRUM;
			}
		}
	}

	int NoteNumberToDifficulty(int noteNumber)
	{
		if (noteNumber >= 58 && noteNumber <= 66)
			return NOTES_EASY;
		else if (noteNumber >= 70 && noteNumber <= 78)
			return NOTES_MEDIUM;
		else if (noteNumber >= 82 && noteNumber <= 90)
			return NOTES_HARD;
		else if (noteNumber >= 94 && noteNumber <= 110)
			return NOTES_EXPERT;
		return -1;
	}

	int NoteNumberToPadDrums(int noteNumber)
	{
		// Happens to nicely convert to what the game expects :D
		int toNote = noteNumber % 12;

		if (toNote > 5) // This is the only difference to the function below
			return 0;

		return toNote;
	}

	int NoteNumberToPad(int noteNumber)
	{
		// Happens to nicely convert to what the game expects :D
		int toNote = noteNumber % 12;

		if (toNote >= 5)
			return 0;
		
		return toNote;
	}

	int NoteNumberToPadSixFret(int noteNumber)
	{
		// weird 6 fret charting conventions 
		int toNote = noteNumber % 12;

		if (toNote == 10) {
			return 7; // Open Note
		}

		if (toNote > 4)
			return 0;

		if (toNote == 4)
			return 8; //8 for black 3 because ghl is weird

		toNote += 1;		

		return toNote;
	}
}
