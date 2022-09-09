#ifndef CHART_H_
#define CHART_H_

#include <string>
#include <vector>

#include "../game/notes/note.hpp"
#include "../game/notes/beatline.hpp"

#define NOTES_EASY 0
#define NOTES_MEDIUM 1
#define NOTES_HARD 2
#define NOTES_EXPERT 3

#define INSTRUMENT_TYPE_GUITAR 0
#define INSTRUMENT_TYPE_BASS 1
#define INSTRUMENT_TYPE_DRUMS 2

/* Converts chart position into time utilizing BPMs
 x = Chart Position
 r = Chart Resolution
 b = BPM (in integers where 120000 = 120)
*/
#define POS_TO_TIME(x,r,b) ((float)(x) / (float)(r)) * (60000.0f / (float)(b));

/* Converts time into chart position utilizing BPMs
 x = Time
 r = Chart Resolution
 b = BPM (in integers where 120000 = 120)
*/
#define TIME_TO_POS(x,r,b) (((float)(b) * (float)(x) * (float)(r)) / 60000.0f)

#define NATURAL_HOPO_DISTANCE(r) (65.0f * (float)(r) / 192.0f)

namespace Rhythmic
{
	const static char *g_instrumentNames[4] = {
		"Guitar",
		"Bass",
		"Drums",
		"Keys"
	};

	constexpr int INSTRUMENT_COUNT = 3;
	constexpr int DIFFICULTY_COUNT = 4;

	constexpr int CHART_NOTE_TRY_HOPO = (1 << 0);
	constexpr int CHART_NOTE_TRY_STRUM = (1 << 1);
	constexpr int CHART_NOTE_TRY_TAP = (1 << 2);
	constexpr int CHART_NOTE_TRY_CYMBAL = (1 << 3);

	enum NotePlayType {
		STRUM,
		HOPO,
		TAP
	};

	struct ChartNote
	{
		int position;
		int note;
		int sustain;
		int tryForce; // Tries to force the note to a certain type (DOES NOT FLIP HOPOs) 1 HOPO, 2 Strum, 3 Tap
	};

	struct ChartBPM
	{
		int position;
		float time;

		int bpm;
	};

	struct ChartTS // Here as a skeleton for a parsing function. Will mess around with this later
	{
		int position;
		float time;

		int numerator = 4;
		int denominator = 4;
	};

	struct ChartSongData
	{
		std::string song_folder;
		std::string name = "Unknown Song";				// The name of the song
		std::string artist = "Unknown Artist";			// The artist of the song
		std::string charter = "Unknown Charter";		// The name of the charted
		float offset = 0;								// This is based off of seconds
		int resolution = 192;							// Resolution of note position (aka, how many units in a beat)
		int sustainScore = (768 / 110);					// Every four beats gives 110. Since the scoring is done by chart position, this value is just the inverse of score/4beats
		bool supportsProDrums = false;					// Does the chart allow for cymbal notes
		//PreviewStart = 0
		//PreviewEnd = 0
		//Genre = "rock"
		//MediaType = "cd"
		//MusicStream = "song.wav"
	};

	struct ChartStarpower
	{
		int position;
		int type;
		int length;

		float timeBegin;
		float timeEnd;
	};

	struct ChartSection
	{
		int position;
		int endPosition;

		float time;
		float endTime;

		std::string name;
	};
	struct Section // A structure for use in the instrument class for stats
	{
		int notesInSection = 0;
		int notesHitInSection = 0;
	};

	struct Chart
	{
		ChartSongData songData;

		std::vector<ChartBPM> listBPM;
		std::vector<ChartTS> listTS;

		std::vector<ChartSection> listSection;

		unsigned int			lastNotePosWithSustain = 0;		// Holds the last position on the chart (note pos + sustain pos)

		std::vector<ChartNote>			notes[INSTRUMENT_COUNT][DIFFICULTY_COUNT];			// Holds all note info (including modifier notes) for all instruments on all difficulties
		unsigned int					notesModifiers[INSTRUMENT_COUNT][DIFFICULTY_COUNT]=
		{{0,0,0,0},{0,0,0,0},{0,0,0,0}};						// Informs the parser how many notes are not playable notes and are used to modify other note flags
		std::vector<ChartStarpower>		spPhrases[INSTRUMENT_COUNT][DIFFICULTY_COUNT];		// The phrases of starpower (each instrument and difficulty is able to have their own)
		bool							supports[INSTRUMENT_COUNT] =
		{0,0,0};												// Will be true if one of the instruments is supported
	};

	inline void ClearChart(Chart &chart)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				chart.notes[i][j].clear();
				chart.notesModifiers[i][j] = 0;
			}
		}

		chart.lastNotePosWithSustain = 0;

		chart.listBPM.clear();
		chart.listTS.clear();

		//chart.songData.artist.clear();
		//chart.songData.charter.clear();
		//chart.songData.name.clear();
		chart.songData.offset = 0;
		chart.songData.resolution = 192;
	}

	ChartBPM GetCurrentBPM(int position, Chart &chart);
	ChartTS GetCurrentTS(int position, Chart &chart);
	ChartBPM GetCurrentBPMByTime(float time, Chart &chart);
	ChartTS GetCurrentTSByTime(float time, Chart &chart);

	int GetSectionIndex(int position, Chart &chart); // Returns an index to a valid section. Will return -1 if section list is empty
	float GetEndSectionTime(int sectionIndex, const Chart &chart); // Returns the sections duration. Will return -1 if section index is out of range
	float GetEndSectionTime(const ChartSection &section, const Chart &chart); // Returns the sections duration. Will return -1 if section is non existent in the given chart

	float GetAdjustedTimeFromPosition(int position, const Chart &chart);
	int GetAdjustedPositionFromTime(float time, const Chart &chart);

	struct ChartToPool
	{
		int difficulty;
		int instrument;
		int asFrets = 5;
		int drumCymbalsLoad;

		int m_baseScore;
		std::vector<Note> *notes;
		std::vector<ChartStarpower> *starpowerPhrases;
		std::vector<Section> *sections;
	};

	/*
	Will take raw notes and change them to game notes. Will also include track based events such as solos and starpower phrases
	*/
	int ChartCalculate(Chart &chart, ChartToPool &desc);

	void GenBeatlines(Chart &chart, std::vector<Beatline> *beatlines);
	void GetCorrectedStarpowerPhrases(std::vector<ChartStarpower> &starpower, const Chart &chart);

	float GetFullMeasureAt(float time, Chart &chart);

}

#endif

