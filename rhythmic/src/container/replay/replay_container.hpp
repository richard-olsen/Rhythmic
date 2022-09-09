#ifndef RHYTHMIC_REPLAY_CONTAINER
#define RHYTHMIC_REPLAY_CONTAINER

#include <vector>
#include "replay.hpp"

namespace Rhythmic
{
	namespace ReplayContainer
	{
		std::vector<Replay> *GetReplays();
		bool isPlayingReplay();
	}
}
#endif