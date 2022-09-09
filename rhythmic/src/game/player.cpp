#include "player.hpp"

#include <algorithm>
#include <iostream>

#include "instrument/instrument_5fret.hpp"
#include "instrument/instrument_drum.hpp"
#include "instrument/instrument_calibrate.hpp"

#include "../container/fretboard/fretboard_container.hpp"
#include "../container/fretboard/fretboard.hpp"

#include "stage/stage.hpp"

#include "../container/player_container.hpp"

#include "../util/misc.hpp"

namespace Rhythmic
{
	

	Player::Player(ContainerPlayer *parent, int index) :
		m_instrument(0),
		m_parent(parent)
	{
		m_fretboard = RenderManager::CreateTexture(0, std::string("player_") + std::to_string(index));
		SetFretboardTexture(FretboardContainer::GetFretboard(parent->m_fretboard));
	}
	Player::~Player()
	{
	}

	void Player::ResetParent(ContainerPlayer *parent)
	{
		m_parent = parent;
	}

	TextureReference Player::GetFretboardTexture()
	{
		return m_fretboard;
	}

	void Player::SetFretboardTexture(Fretboard *fretboard)
	{
		if (fretboard == 0)
			m_fretboard->UpdateTexture(0);
		else
		{
			IO_Image image;
			IO_LoadImage(Util::GetExecutablePath() + "/fretboards/" + fretboard->m_fileName, &image);
			m_fretboard->UpdateTexture(&image);
			IO_FreeImageData(&image);
		}		
	}

	void Player::EstablishPlayer()
	{
		if (m_instrument) delete m_instrument;
		
		if (m_parent->m_instrument == -1) // Used for calibration
			m_instrument = new InstrumentCalibrate();
		else if (m_parent->m_instrument == INSTRUMENT_TYPE_GUITAR || m_parent->m_instrument == INSTRUMENT_TYPE_BASS)
			m_instrument = new Instrument5Fret();
		else if (m_parent->m_instrument == INSTRUMENT_TYPE_DRUMS)
			m_instrument = new InstrumentDrums();

		m_instrument->Create(m_parent);

		StageSystem::GetInstance()->EstablishPlayer(this);
	}
	
	void Player::DisestablishPlayer()
	{
		if (m_instrument)
		{
			m_instrument->Destroy();
			delete m_instrument;
			m_instrument = 0;
		}
		//m_fretboard.Dispose();
		StageSystem::GetInstance()->DisestablishPlayer(this);
		//m_stage = 0;
	}

	bool Player::IsPlayerEstablished()
	{
		//return m_stage != 0;
		return StageSystem::GetInstance()->IsPlayerEstablished(this);
	}

	void Player::Update(float delta, float timeOffset)
	{
		if (m_instrument) m_instrument->Update(delta, timeOffset);
	}

	Instrument *Player::GetInstrument()
	{
		return m_instrument;
	}

	PlayerSettings *Player::GetSettings()
	{
		return &m_parent->m_settings;
	}
	ContainerPlayer *Player::GetContainer()
	{
		return m_parent;
	}
	void Player::SetSettings(PlayerSettings *settings)
	{
		
	}
}

