#ifndef MIDI_PARSER_H_
#define MIDI_PARSER_H_

#include "chart.hpp"

#include <string>

namespace Rhythmic
{
	bool LoadChartFromMidi(Chart *chart, const std::string &chart_file_name, bool loadHeaderOnly = false);
}

#endif

