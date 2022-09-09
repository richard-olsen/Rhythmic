#ifndef CHART_PARSER_H_
#define CHART_PARSER_H_

#include "chart.hpp"

#include <string>

namespace Rhythmic
{
	bool LoadChartFromChart(Chart *chart, const std::string &chart_file_name, bool loadHeaderOnly = false, bool useChartHeaderAsSongInfo = false);
}

#endif

