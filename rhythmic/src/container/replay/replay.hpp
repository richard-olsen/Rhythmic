#ifndef RHYTHMIC_REPLAY
#define RHYTHMIC_REPLAY

#include <vector>

#include "../../container/player_container.hpp"
#include "../../container/song_container.hpp"

#include "../../input/input_structures.hpp"

namespace Rhythmic
{
	struct Input
	{
		InputEventData *m_data;
		ContainerPlayer m_player;
		float m_songTime;
	};

	class Replay
	{
	public:
		ContainerSong m_song;
		std::vector<Input> m_inputs;
		std::vector<ContainerPlayer> m_players;
	};
	
}
#endif