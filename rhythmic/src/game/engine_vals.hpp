#ifndef RHYTHMIC_ENGINE_VALS
#define RHYTHMIC_ENGINE_VALS

namespace Rhythmic
{
	namespace GamePlayVariables
	{
		// Original struct with original values
		//struct EngineFret5Variables
		//{
		//	int m_rangeFrontEnd = 100;					// Front End time to get Hopos/Taps
		//	bool m_rangeFrontEndInfinite = false;		// Toggles an infinite front end
		//	int m_rangeCatcherTap = 60;				// Hopo/Tapping range
		//	int m_rangeCatcherStrum = 140;				// Strumming range
		//	int m_strumLeniency = 50;					// Strum leniency (allowing the player to hit a note when they strummed before mashing the button down)
		//	int m_hopoLeniency = 30;					// Hopo leniency (allowing the player to fret off, and still hit the note if it's distance falls within the timer)
		//	int m_timeTapIgnoreOverstrum = 150;			// A constant for allowing players to strum on a Hopo and not break their combo. This does not automatically give the strum to the hopo; if there is a note the player can hit in front of the hopo, that note will suck the strum before the tap/hopo does
		//};
		struct EngineFret5Variables
		{
			int m_hopoFrontEnd = 50;					// Front End time to get Hopos/Taps
			bool m_infiniteHOPOFrontEnd = false;		// Toggles an infinite front end - 1 byte
			int m_hopoTapRange = 40;					// Hopo/Tapping front range (This is the front half, the back half will be the same as strumming)
			int m_strumRange = 150;						// Strumming range

			int m_strumLeniency = 50;					// Strum leniency (allowing the player to hit a note when they strummed before mashing the button down)
			int m_hopoLeniency = 30;					// Hopo leniency (allowing the player to fret off, and still hit the note if it's distance falls within the timer)
			int m_timeTapIgnoreOverstrum = 150;			// A constant for allowing players to strum on a Hopo and not break their combo. This does not automatically give the strum to the hopo; if there is a note the player can hit in front of the hopo, that note will suck the strum before the tap/hopo does

			bool m_antiGhost = true;
		};
		struct EngineDrumsVariables
		{
			int m_rangeCatcher = 120;
			int m_padActive = 50;						// !Visual Variable! Determines how long the pad looks active for
			int m_chordTime = 120;						// The time allowed to hit a chord before the combo breaks
			int m_spActivationTime = 120;				// The time allowed to activate starpower before doing nothing, or breaking combo
		};

		struct ChartModifierVariables {
			// Guitar/Drum
			bool m_allStrums = false;
			bool m_allHopos = false;
			bool m_allTaps = false;
			bool m_hoposToTaps = false;
			bool m_noteShuffle = false;
			bool m_mirrorMode = false;
			bool m_doubleNotes = false;

			// Drum Only
			bool m_allCymbals = false;
			bool m_shuffleCymbals = false;
			
			int m_songSpeed = 100;
		};

		extern ChartModifierVariables g_chartModifiers;
		extern EngineFret5Variables g_engineFret5;
		extern EngineDrumsVariables g_engineDrums;
	}
}

#endif

