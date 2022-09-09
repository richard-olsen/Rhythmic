#ifndef SONG_H_
#define SONG_H_

#include "chart.hpp"

#include "chart_parser.hpp"
#include "midi_parser.hpp"

#define SONG_UNKNOWN_SONG "Unknown Song"
#define SONG_UNKNOWN_ARTIST "Unknown Artist"
#define SONG_UNKNOWN_CHARTER "Unknown Charter"

#define SONG_KEY_NAME "name"
#define SONG_KEY_ARTIST "artist"
#define SONG_KEY_CHARTER "charter"
#define SONG_KEY_SONG_LENGTH "song_length"
#define SONG_KEY_PREVIEW_START "preview_start_time"
#define SONG_KEY_PRO_DRUMS "pro_drums"

namespace Rhythmic
{
	Chart LoadChart(const std::string &chart);
}

#endif

