#ifndef PLAYER_H_
#define PLAYER_H_

#include "../container/fretboard/fretboard.hpp"

#include "catchers/catcher_manager.hpp"

#include "../rendering/texture.hpp"

#include "notes/note_renderer.hpp"

#include "../chart/chart.hpp"

#include <vector>

#include "instrument/instrument.hpp"

#include "../input/input.hpp"

#include "../util/player_settings.hpp"

namespace Rhythmic
{
	struct ContainerPlayer;
	class Player
	{
	public:
		Player(ContainerPlayer *parent, int index = -1);
		~Player();

		void ResetParent(ContainerPlayer *parent);

		void SetFretboardTexture(Fretboard *fretboard);
		TextureReference GetFretboardTexture();

		void EstablishPlayer();
		void DisestablishPlayer();

		bool IsPlayerEstablished();

		void Update(float delta, float timeOffset);

		Instrument *GetInstrument();

		PlayerSettings *GetSettings();

		ContainerPlayer *GetContainer();

		void SetSettings(PlayerSettings *settings);

	private:
		ContainerPlayer		*m_parent;
		Instrument			*m_instrument;

		TextureReference	 m_fretboard;
	};
}

#endif

