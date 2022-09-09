#ifndef RHYTHMIC_ENGINE_H
#define RHYTHMIC_ENGINE_H

#include <vector>
#include <string>
#include <wge/util/guid.hpp>

namespace Rhythmic
{

	struct EngineFret5Variables // Size: 26 bytes
	{
		int m_hopoFrontEnd = 50;					// Front End time to get Hopos/Taps
		bool m_infiniteHOPOFrontEnd = false;		// Toggles an infinite front end
		int m_hopoTapRange = 40;					// Hopo/Tapping front range (This is the front half, the back half will be the same as strumming)
		int m_strumRange = 150;						// Strumming range

		int m_strumLeniency = 50;					// Strum leniency (allowing the player to hit a note when they strummed before mashing the button down)
		int m_hopoLeniency = 30;					// Hopo leniency (allowing the player to fret off, and still hit the note if it's distance falls within the timer)
		int m_timeTapIgnoreOverstrum = 150;			// A constant for allowing players to strum on a Hopo and not break their combo. This does not automatically give the strum to the hopo; if there is a note the player can hit in front of the hopo, that note will suck the strum before the tap/hopo does

		bool m_antiGhost = false;
	};
	struct EngineDrumsVariables // Size: 16 bytes
	{
		int m_rangeCatcher = 150;
		int m_padActive = 50;						// !Visual Variable! Determines how long the pad looks active for
		int m_chordTime = 120;						// The time allowed to hit a chord before the combo breaks
		int m_spActivationTime = 120;				// The time allowed to activate starpower before doing nothing, or breaking combo
	};
	struct EngineKeysVariables 
	{
		// Add when keys made
	};

	struct Engine
	{
		EngineFret5Variables m_fret5;
		EngineDrumsVariables m_drums;
		EngineKeysVariables  m_keys;

		std::string m_name;
		std::string m_author;
		int m_version;
		bool m_isCustom = false;

		std::string m_filename;
		WGE::Util::GUID m_guid;
	};

}
#endif