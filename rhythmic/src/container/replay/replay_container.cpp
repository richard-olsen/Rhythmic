#include <vector>

#include "replay_container.hpp"
#include "replay.hpp"

namespace Rhythmic
{
	namespace ReplayContainer
	{
		std::vector<Replay> g_replays;
		std::vector<Replay> *GetReplays() { return &g_replays; }

		bool g_isPlayingReplay;
		bool isPlayingReplay() { return g_isPlayingReplay; }

		bool LoadReplay(bool full)
		{
			return true;
		}



	}
}