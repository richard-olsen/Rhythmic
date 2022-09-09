#include "chart_parser.hpp"

#include <fstream>

#include <wge/core/logging.hpp>

#include "../util/file_util.hpp"
#include "../util/misc.hpp"

namespace Rhythmic
{
	/*
	Controls the flow of the parser
	*/
	enum class ChartFlow
	{
		UNSCOPED,
		INI,
		NORMAL,
		DISREGARD
	};

	/*
	A structure representing the events
	of the chart file
	*/
	struct ChartData
	{
		// 0 = TS 4 4
		// 0 = B 120000
		// 1280 = E "section Oh Yea"
		// 2830 = N 0 192

		int position = 0;		// 1280

		char type = -1;			// 'N' or 'S' or 'E' etc.
		
		std::string value;	// Used for E type like Sections etc.

		int valLeft = 0;		// Used for notes or flags
		int valRight = 0;		// The length
	};
	
	/*
	Figures out which instrument the parser
	is going to parse next, controlling the
	flow of the parser
	*/
	void DetermineInstrument(
		Chart *chart,
		const std::string &currentLine,
		int &targetInstrument,
		int &targetDifficulty,
		ChartFlow &nextFlow
	)
	{
		/*
		First, the difficulty will be determined
		*/
		std::string_view section = currentLine;
		section = section.substr(1, section.size() - 2);

		switch (section[0])
		{
		case 'M':
			targetDifficulty = NOTES_MEDIUM;
			section = section.substr(6);
			break;
		case 'H':
			targetDifficulty = NOTES_HARD;
			section = section.substr(4);
			break;
		case 'E':
			switch (section[1])
			{
			case 'a':
				targetDifficulty = NOTES_EASY;
				section = section.substr(4);
				break;
			case 'x':
				targetDifficulty = NOTES_EXPERT;
				section = section.substr(6);
				break;
			}
			break;
		}

		if (section == "Single")
		{
			chart->supports[INSTRUMENT_TYPE_GUITAR] = true;
			targetInstrument = INSTRUMENT_TYPE_GUITAR;
			nextFlow = ChartFlow::NORMAL;
		}
		else if (section == "DoubleBass")
		{
			chart->supports[INSTRUMENT_TYPE_BASS] = true;
			targetInstrument = INSTRUMENT_TYPE_BASS;
			nextFlow = ChartFlow::NORMAL;
		}
		else if (section == "Drums")
		{
			chart->supports[INSTRUMENT_TYPE_DRUMS] = true;
			targetInstrument = INSTRUMENT_TYPE_DRUMS;
			nextFlow = ChartFlow::NORMAL;
		}
	}

	/*
	Process all E type of events
	*/
	void ProcE(Chart *chart, ChartData &data)
	{
		size_t firstSpace = data.value.find_first_of(' ');
		std::string type = data.value.substr(0, firstSpace);

		if (Util::compareStringCaseInsensitive(type, "section") == 0)
		{
			ChartSection section;
			section.position = data.position;
			section.name = data.value.substr(firstSpace + 1);
			chart->listSection.push_back(section);
		}
	}

	/*
	Process all N type of events
	*/
	void ProcN(Chart *chart, int instrument, int difficulty, ChartData &data)
	{
		ChartNote note;

		note.position = data.position;
		note.note = data.valLeft;
		note.sustain = data.valRight;
		note.tryForce = 0;

		int ps = note.position + note.sustain;
		if (chart->lastNotePosWithSustain < ps)
			chart->lastNotePosWithSustain = ps;

		chart->notes[instrument][difficulty].push_back(note);

		/*
		Each instrument defines their
		note modifiers differently
		*/
		switch (instrument)
		{
		case INSTRUMENT_TYPE_GUITAR:
		case INSTRUMENT_TYPE_BASS:
			if (note.note >= 5 && note.note != 7)
				chart->notesModifiers[instrument][difficulty]++;
			break;
		case INSTRUMENT_TYPE_DRUMS:
			if (note.note > 5)
				chart->notesModifiers[instrument][difficulty]++;
			break;
		}
	}

	/*
	Process all S type of events
	*/
	void ProcS(Chart *chart, int instrument, int difficulty, ChartData &data)
	{
		/*
		2 is common between ALL instruments for
		SP phrases
		*/
		if (data.valLeft == 2)
		{
			ChartStarpower phrase;
			phrase.position = data.position;
			phrase.length = data.valRight;

			chart->spPhrases[instrument][difficulty].push_back(phrase);

			return;
		}

		/*
		Some instruments have extra S events
		defined for them. They go here
		*/
		/*switch (instrument)
		{
		case INSTRUMENT_TYPE_GUITAR:
		case INSTRUMENT_TYPE_BASS:
			break;
		case INSTRUMENT_TYPE_DRUMS:
			break;
		}*/
	}

	/*
	Process the events of the chart for
	notes, tempo mapping, etc.
	*/
	void ProcEvent(Chart *chart, int instrument, int difficulty, ChartData &data)
	{
		switch (data.type)
		{
			/*
			Events
			*/
		case 'E':
			ProcE(chart, data);
			break;
			/*
			Tempo Mapping
			*/
		case 'T': // Should compare as TS as in spec
		{
			int numerator = data.valLeft;
			int denominator = 4;

			if (data.valRight > 0)
				denominator = (int)std::log2(data.valRight);

			chart->listTS.push_back({ data.position, 0.0f, numerator, denominator });
		}
			break;
		case 'B':
			chart->listBPM.push_back({ data.position, 0.0f, data.valLeft });
			break;
			/*
			Actual notes and stuff
			*/
		case 'N':
			ProcN(chart, instrument, difficulty, data);
			break;
		case 'S':
			ProcS(chart, instrument, difficulty, data);
			break;
		}
	}

	bool LoadChartFromChart(Chart *chart, const std::string &chart_file_name, bool loadHeaderOnly, bool useChartHeaderAsSongInfo)
	{
		std::ifstream chartFile(chart_file_name);
		if (!chartFile.is_open())
		{
			LOG_ERROR("Failed to load chart [{0}]", chart_file_name);
			return false;
		}

		/*
		Check file encoding with the BOM

		Return if the file encoding isn't
		supported
		*/
		bool hasBOM = false;
		FileEncoding encoding = FileUtil::CheckBOM(chartFile, &hasBOM);
		if (encoding != FileEncoding::FILE_ENC_UTF8)
		{
			LOG_ERROR("File Encoding Not Supported [{0}]", chart_file_name);
			return false;
		}

		std::string currentLine;
		ChartFlow flow = ChartFlow::UNSCOPED;
		ChartFlow nextFlow = ChartFlow::DISREGARD;

		ChartData chartData;

		int targetInstrument = -1;
		int targetDifficulty = -1;

		while (chartFile.good())
		{
			std::getline(chartFile, currentLine);

			Util::Trim(currentLine);
			
			if (currentLine.empty())
				continue;

			if (currentLine[0] == '}')
			{
				if (flow == ChartFlow::INI && loadHeaderOnly)
					return true;

				flow = ChartFlow::UNSCOPED;
				continue;
			}

			switch (flow)
			{
			case ChartFlow::UNSCOPED:
				if (currentLine[0] == '{')
				{
					flow = nextFlow;
					nextFlow = ChartFlow::DISREGARD;
					break;
				}
				
				if (currentLine == "[Song]")
				{
					nextFlow = ChartFlow::INI;
					break;
				}

				if (
					currentLine == "[SyncTrack]" ||
					currentLine == "[Events]"
					)
				{
					nextFlow = ChartFlow::NORMAL;
					break;
				}

				DetermineInstrument(chart, currentLine, targetInstrument, targetDifficulty, nextFlow);

				break;
			case ChartFlow::INI: // The only portion using this is [Song]
			{
				size_t splitter = currentLine.find_first_of('=');

				std::string type = currentLine.substr(0, splitter);
				Util::Trim(type);

				std::string value = currentLine.substr(splitter + 1);
				Util::Trim(value);

				value = Util::RemoveQuotes(value);

				if (Util::compareStringCaseInsensitive(type, "offset") == 0)
					chart->songData.offset = std::stof(value);

				else if (Util::compareStringCaseInsensitive(type, "resolution") == 0)
					chart->songData.resolution = std::stoi(value);

				// Now go for the strings

				else if (Util::compareStringCaseInsensitive(type, "name") == 0 && useChartHeaderAsSongInfo)
					chart->songData.name = value;
				else if (Util::compareStringCaseInsensitive(type, "artist") == 0 && useChartHeaderAsSongInfo)
					chart->songData.artist = value;
				else if (Util::compareStringCaseInsensitive(type, "charter") == 0 && useChartHeaderAsSongInfo)
					chart->songData.charter = value;
			}
				break;
			case ChartFlow::NORMAL:
			{
				std::vector<std::string> tokens;
				Util::Split(currentLine, "=", tokens);

				/*
				Skip if the size isn't correct!
				It represents the overlying string of
				        |
				"102934 = TS 4 2"
				        |
						Splits here
				*/
				if (tokens.size() != 2)
					break;

				std::string &position = tokens[0];
				std::string &otherVals = tokens[1];

				Util::Trim(position);
				Util::Trim(otherVals);

				chartData.position = std::stoi(position);
				chartData.type = otherVals[0];

				/*
				For E events, grab the data inside
				the string, which should be wrapped in
				quotations
				*/
				if (chartData.type == 'E')
				{
					size_t fPos = otherVals.find_first_of('\"') + 1;
					size_t lPos = otherVals.find_last_of('\"');

					chartData.value = otherVals.substr(fPos, (lPos - fPos));
				}
				else
				{
					std::vector<std::string> values;
					Util::Split(otherVals, " ", values);

					size_t size = values.size();

					if (size < 2)
						break;

					chartData.valLeft = std::stoi(values[1]);

					if (size > 2)
						chartData.valRight = std::stoi(values[2]);
					else
						chartData.valRight = 0;
				}

				ProcEvent(chart, targetInstrument, targetDifficulty, chartData);
			}
				break;
			}
		}

		/*
		Fill in BPM and TS incase the chart
		contained none
		*/

		if (chart->listBPM.empty())
			chart->listBPM.push_back({ 0, 0.0f, 120000 });

		if (chart->listTS.empty())
			chart->listTS.push_back({ 0, 0.0f, 4, 4 });

		return true;
	}
}