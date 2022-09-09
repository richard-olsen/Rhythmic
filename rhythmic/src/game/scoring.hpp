#ifndef RHYTHMIC_SCORING_HPP_
#define RHYTHMIC_SCORING_HPP_

#include "../chart/chart.hpp"

namespace Rhythmic
{
	constexpr int SCORE_PER_NOTE = 50;			// Given every note hit
	constexpr int SCORE_GIVEN_PER_TICK = 110;	// Helps get the Chart Pos for every 1 point

	inline int ScoreCalculateFull(Chart *chart, unsigned int len)
	{
		return (len / chart->songData.sustainScore);
	}
}

#endif

