#include "chart.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include "../util/settings.hpp"
#include "../game/instrument/instrument.hpp"

#include <math.h>

#include "../game/scoring.hpp"

#include <wge/core/logging.hpp>

namespace Rhythmic
{
	ChartBPM GetCurrentBPM(int position, Chart &chart)
	{
		ChartBPM currentBPM = chart.listBPM[0];
		for (int j = 1; j < chart.listBPM.size(); j++)
		{
			if (position >= chart.listBPM[j].position)
			{
				currentBPM = chart.listBPM[j];
			}
			else
				break;
		}
		return currentBPM;
	}

	ChartTS GetCurrentTS(int position, Chart &chart)
	{
		ChartTS currentTS;
		if (chart.listTS.empty())
			currentTS = { 0, 0, 4 };
		else
			currentTS = chart.listTS[0];
		for (int j = 1; j < chart.listTS.size(); j++)
		{
			if (position >= chart.listTS[j].position)
			{
				currentTS = chart.listTS[j];
			}
			else
				break;
		}
		return currentTS;
	}

	ChartBPM GetCurrentBPMByTime(float time, Chart &chart)
	{
		ChartBPM currentBPM = chart.listBPM[0];
		for (int j = 1; j < chart.listBPM.size(); j++)
		{
			if (time >= chart.listBPM[j].time)
			{
				currentBPM = chart.listBPM[j];
			}
			else
				break;
		}
		return currentBPM;
	}
	ChartTS GetCurrentTSByTime(float time, Chart &chart)
	{
		ChartTS currentTS = chart.listTS[0];
		for (int j = 1; j < chart.listTS.size(); j++)
		{
			if (time >= chart.listTS[j].time)
			{
				currentTS = chart.listTS[j];
			}
			else
				break;
		}
		return currentTS;
	}

	int GetSectionIndex(int position, Chart &chart)
	{
		if (chart.listSection.empty())
			return -1;

		int ret = 0;

		for (int i = 0; i < chart.listSection.size(); i++)
		{
			if (position >= chart.listSection[i].position)
			{
				ret = i;
				continue;
			}
			break;
		}

		return ret;
	}

	float GetEndSectionTime(int sectionIndex, const Chart &chart)
	{
		if (sectionIndex < 0 || sectionIndex >= chart.listSection.size())
			return -1;

		if (sectionIndex == chart.listSection.size() - 1)
			return GetAdjustedTimeFromPosition(chart.lastNotePosWithSustain, chart);

		return GetAdjustedTimeFromPosition(chart.listSection[sectionIndex + 1].position, chart);
	}
	float GetEndSectionTime(const ChartSection &section, const Chart &chart)
	{
		for (int i = 0; i < chart.listSection.size(); i++)
		{
			if ((section.position == chart.listSection[i].position) &&
				(section.name == chart.listSection[i].name))
				return GetEndSectionTime(i, chart);
		}
		return -1;
	}

	float GetAdjustedTimeFromPosition(int position, const Chart &chart)
	{
		int posOffset = 0;
		float timeOffset = 0;
		ChartBPM currentBPM = chart.listBPM[0];
		for (int j = 1; j < chart.listBPM.size(); j++)
		{
			if (position >= chart.listBPM[j].position)
			{
				int prevBPM = currentBPM.bpm;
				currentBPM = chart.listBPM[j];
				timeOffset += POS_TO_TIME(currentBPM.position - posOffset, chart.songData.resolution, prevBPM);
				posOffset = currentBPM.position;
			}
			else
				break;
		}

		return timeOffset += POS_TO_TIME(position - posOffset, chart.songData.resolution, currentBPM.bpm);
	}

	int GetAdjustedPositionFromTime(float time, const Chart &chart)
	{
		if (time <= 0) return 0;

		ChartBPM bpm;

		for (int i = 0; i < chart.listBPM.size(); i++)
		{
			if (chart.listBPM[i].time > time)
				break;
			bpm = chart.listBPM[i];
		}

		return bpm.position += TIME_TO_POS((time - bpm.time), chart.songData.resolution, bpm.bpm);
	}
	
	struct _sort_content_note
	{
		bool operator()(const Note &item1, const Note &item2)
		{
			if (item1.time < item2.time)
				return true;
			return item1.note < item2.note;
		}
	};

	struct CalculateInstrument
	{
		Chart *chart;
		ChartToPool *desc;

		std::vector<ChartNote> *modifiers;

		int loadCymbals;
		int fretCount;

		int index;
		bool spNote;
		int notesToCount;
		int chord;
		int position;

		int hopoRequirement;

		float audioCalibration;
	};

	int _calculate5Fret(CalculateInstrument &values)
	{
		int target = 0;
		
		auto modifier = values.modifiers->begin();

		Note &baseNote = (*values.desc->notes)[values.index];

		while (modifier != values.modifiers->end())
		{
			if (modifier->position == baseNote.chartPos)
			{
				if (modifier->note == 5) // Force Flag
					target = 1;
				if (modifier->note == 6) // Tap Flag
					target = 2;

				modifier = values.modifiers->erase(modifier);
				continue;
			}

			if (modifier->position > baseNote.chartPos)
				break;

			modifier++;
		}

		int largestSustain = 0;

		for (int n = 0; n < values.chord; n++)
		{
			Note &note = (*values.desc->notes)[values.index + n];

			if (note.chartLen > largestSustain)
				largestSustain = note.chartLen;

			note.time = GetAdjustedTimeFromPosition(note.chartPos, *values.chart) + values.audioCalibration + values.chart->songData.offset;
			note.sustain = GetAdjustedTimeFromPosition(note.chartLen + note.chartPos, *values.chart) + values.audioCalibration + values.chart->songData.offset;
			values.position = note.chartPos;

			if (values.chord > 1)
				SET_NOTE(note, NOTE_FLAG_IS_CHORD);

			if (values.spNote)
				SET_NOTE(note, NOTE_FLAG_IS_STARPOWER);

			if (target == 2)
			{
				SET_NOTE(note, NOTE_FLAG_TAP);
				continue;
			}

			// Forcing the note just flips it's hopo status
			int flagTargetModifiedHopo = target == 1 ? NOTE_FLAG_STRUM : NOTE_FLAG_HOPO;
			int flagTargetModifiedStrum = target == 1 ? NOTE_FLAG_HOPO : NOTE_FLAG_STRUM;

			if (note.chartTarget == 0)// A previous note exists
			{
				if (values.index - 1 < 0)
				{
					SET_NOTE(note, flagTargetModifiedStrum);
					continue;
				}
				// Define rules for HOPOs
				Note &prevNote = (*values.desc->notes)[values.index - 1];

				if (values.chord == 1)
				{
					if ((note.chartPos - prevNote.chartPos <= values.hopoRequirement))
					{
						if (IS_NOTE(prevNote, NOTE_FLAG_IS_CHORD))
							SET_NOTE(note, flagTargetModifiedHopo);
						else if (prevNote.note != note.note)
							SET_NOTE(note, flagTargetModifiedHopo);
						else
							SET_NOTE(note, flagTargetModifiedStrum);
					}
					else
					{
						SET_NOTE(note, flagTargetModifiedStrum);
					}
				}
				else
				{
					SET_NOTE(note, flagTargetModifiedStrum);
				}
			}
			else if (note.chartTarget & CHART_NOTE_TRY_TAP)
			{
				SET_NOTE(note, NOTE_FLAG_TAP);
			}
			else if (note.chartTarget & CHART_NOTE_TRY_STRUM)
			{
				SET_NOTE(note, NOTE_FLAG_STRUM);
			}
			else if (note.chartTarget & CHART_NOTE_TRY_HOPO)
			{
				SET_NOTE(note, NOTE_FLAG_HOPO);
			}
		}

		values.desc->m_baseScore += (values.chord * SCORE_PER_NOTE) + (largestSustain > 0 ? ScoreCalculateFull(values.chart, largestSustain) : 0);

		return 1;
	}
	int _calculateDrums(CalculateInstrument &values)
	{
		for (int n = 0; n < values.chord; n++)
		{
			Note &note = (*values.desc->notes)[values.index + n];

			note.time = GetAdjustedTimeFromPosition(note.chartPos, *values.chart) + values.audioCalibration + values.chart->songData.offset;
			note.sustain = GetAdjustedTimeFromPosition(note.chartLen + note.chartPos, *values.chart) + values.audioCalibration + values.chart->songData.offset;

			values.position = note.chartPos;

			if (values.spNote)
				SET_NOTE(note, NOTE_FLAG_IS_STARPOWER);

			if (values.chord > 1)
				SET_NOTE(note, NOTE_FLAG_IS_CHORD);

			SET_NOTE(note, NOTE_FLAG_STRUM);

			if ((values.fretCount == 5) && (note.note == NOTE_2 || note.note == NOTE_4))
				SET_NOTE(note, NOTE_FLAG_CYMBAL);
			else if ((values.fretCount == 4) && (note.chartTarget & CHART_NOTE_TRY_CYMBAL) && (values.loadCymbals & 1 << (note.note)))
				SET_NOTE(note, NOTE_FLAG_CYMBAL);

			if ((values.fretCount == 4) && note.note == NOTE_5)
			{
				if (n - 1 < 0)
					note.note = NOTE_4;
				else
				{
					Note &lastNote = (*values.desc->notes)[values.index + n - 1];
					if (lastNote.note == NOTE_4)
						note.note = NOTE_3;
					else
						note.note = NOTE_4;
				}
			}
		}
		return values.chord;
	}

	bool _insideStarpower(std::vector<ChartStarpower> *phrases, unsigned int chartPos)
	{
		if (!phrases)
			return false;

		for (int i = 0; i < phrases->size(); i++)
		{
			const ChartStarpower &phrase = (*phrases)[i];
			if ((chartPos >= phrase.position) && (chartPos < (phrase.position + phrase.length)))
				return true;
		}
		return false;
	}

	int ChartCalculate(Chart &chart, ChartToPool &desc)
	{
		assert(desc.notes != nullptr && "A valid note container must be passed!");

		SettingValue ac;
		ac.type = SETTING_GAME_CALIBRATION_AUDIO;
		SettingsGet(&ac);
		float audioCalibration = ac.m_iValue * 0.001f;

		float lastNotePos = -1;
		int noteCount = 0;

		unsigned int hopoRequirement = (unsigned int)(NATURAL_HOPO_DISTANCE(chart.songData.resolution));

		std::vector<ChartNote> modifiers; // List of modifiers to apply to notes afterwards
		std::vector<ChartNote> &notes = chart.notes[desc.instrument][desc.difficulty];

		for (int i = 0; i < notes.size(); i++)
		{
			ChartNote &cNote = notes[i];

			Note note;
			note.chartPos = cNote.position;
			note.chartLen = cNote.sustain;
			note.chartTarget = cNote.tryForce;
			note.flags = 0;
			note.lastScoreCheck = 0;

			switch (desc.instrument)
			{
			case INSTRUMENT_TYPE_GUITAR:
			case INSTRUMENT_TYPE_BASS:
				if (cNote.note == 5 || cNote.note == 6)
				{
					modifiers.push_back(cNote);
				}
				else if (cNote.note <= 4 || cNote.note == 7)
				{
					note.note = cNote.note == 7 ? NOTE_OPEN : cNote.note;
					desc.notes->push_back(note);
				}
				break;
			case INSTRUMENT_TYPE_DRUMS:
				note.note = cNote.note == 0 ? NOTE_OPEN : cNote.note - 1;
				desc.notes->push_back(note);
				break;
			}
		}

		// Sort through the notes and organize them for ease of use.
		// The game expects the notes to be ordered from
		// Lowest Chart Position to Highest, and (if chart pos is the same)
		//		lowest note to highest
		if (desc.notes->size() != 0)
		{
			std::sort(desc.notes->begin(), desc.notes->end(), [](const Note &note1, const Note &note2)
				{
					if (note1.chartPos < note2.chartPos)
						return true;
					else if (note1.chartPos > note2.chartPos)
						return false;
					else
						return (int)note1.note < (int)note2.note;
				});
		}

		CalculateInstrument ci;
		ci.chart = &chart;
		ci.desc = &desc;
		ci.audioCalibration = audioCalibration;
		ci.modifiers = &modifiers;
		ci.hopoRequirement = hopoRequirement;
		ci.fretCount = desc.asFrets;
		ci.loadCymbals = desc.drumCymbalsLoad;

		ci.index = 0;
		while (true)
		{
			if (ci.index >= desc.notes->size())
				break;

			ci.spNote = _insideStarpower(desc.starpowerPhrases, (*desc.notes)[ci.index].chartPos);

			ci.notesToCount = 1;
			ci.chord = 1;
			ci.position = -1;

			while (ci.index + ci.chord < desc.notes->size() && (*desc.notes)[ci.index].chartPos == (*desc.notes)[ci.index + ci.chord].chartPos)
				ci.chord++;

			int notesToCount = 1;

			switch (desc.instrument)
			{
			case INSTRUMENT_TYPE_GUITAR:
			case INSTRUMENT_TYPE_BASS:
				notesToCount = _calculate5Fret(ci);
				break;
			case INSTRUMENT_TYPE_DRUMS:
				notesToCount = _calculateDrums(ci);
				break;
			}

			noteCount += notesToCount;
			ci.index += ci.chord;

			if (ci.position != -1)
			{
				int index = GetSectionIndex(ci.position, chart);
				(*desc.sections)[index].notesInSection += notesToCount;
			}
		}

		return noteCount;
	}

	void GenBeatlines(Chart &chart, std::vector<Beatline> *beatlines)
	{
		SettingValue ac;
		ac.type = SETTING_GAME_CALIBRATION_AUDIO;
		SettingsGet(&ac);
		float audioCalibration = ac.m_iValue * 0.001f;

		beatlines->clear();

		int lastTime = chart.lastNotePosWithSustain;
		if (lastTime % chart.songData.resolution != 0)
		{
			float quotient = (float)lastTime / (float)chart.songData.resolution;

			float addAmount = (1.0 - (quotient - floor(quotient)));

			lastTime += chart.songData.resolution * addAmount;
		}


		ChartTS lastTS = GetCurrentTS(0, chart);
		unsigned int numeratorCounter = lastTS.numerator - 1;

		int time = 0;

		while (time <= lastTime)
		{
			Beatline beatline = Beatline();
			beatline.time = GetAdjustedTimeFromPosition(time, chart) + audioCalibration + chart.songData.offset;

			// Get the next time signature

			ChartTS ts = GetCurrentTS(time, chart);

			if (lastTS.numerator != ts.numerator || lastTS.denominator != ts.denominator)
			{
				numeratorCounter = ts.numerator - 1;
				lastTS = ts;
			}

			// Now determine which one to use

			if (numeratorCounter >= (lastTS.numerator - 1))
			{
				beatline.type = BEATLINE_TYPE_FULL;
				numeratorCounter = 0;
			}
			else
			{
				beatline.type = BEATLINE_TYPE_HALF;
				numeratorCounter++;
			}

			beatlines->push_back(beatline);

			time += (chart.songData.resolution * 4) / (lastTS.denominator <= 0 ? 4 : lastTS.denominator);
		}
	}

	void GetCorrectedStarpowerPhrases(std::vector<ChartStarpower> &starpower, const Chart &chart)
	{
		SettingValue ac;
		ac.type = SETTING_GAME_CALIBRATION_AUDIO;
		SettingsGet(&ac);
		float audioCalibration = ac.m_iValue * 0.001f;
		for (auto it = starpower.begin(); it != starpower.end(); it++)
		{
			it->timeBegin = (it->timeBegin + audioCalibration + chart.songData.offset);
			it->timeEnd = (it->timeEnd + audioCalibration + chart.songData.offset);
		}
	}

	float GetBeatAt(float time, Chart &chart)
	{
		ChartTS currentTS = GetCurrentTSByTime(time, chart);
		int fullMeasure = (chart.songData.resolution * currentTS.denominator);
		ChartBPM currentBPM = GetCurrentBPMByTime(time, chart);
		return POS_TO_TIME(fullMeasure, chart.songData.resolution, currentBPM.bpm);
	}

	float GetFullMeasureAt(float time, Chart &chart)
	{
		ChartTS currentTS = GetCurrentTSByTime(time, chart);
		int fullMeasure = (chart.songData.resolution * (currentTS.denominator == 0 ? 4 : currentTS.denominator));
		ChartBPM currentBPM = GetCurrentBPMByTime(time, chart);
		return POS_TO_TIME(fullMeasure, chart.songData.resolution, currentBPM.bpm);
	}

	
}
