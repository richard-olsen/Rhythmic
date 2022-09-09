#ifndef INSTRUMENT_DRUM_H_
#define INSTRUMENT_DRUM_H_

#include "instrument.hpp"

#include "../../util/misc.hpp"
#include <wge/core/logging.hpp>

// Before implementing the drums idea list
/*
	- Have catchers active for a certain amount of time (kick included). Quarter of a second?
	- If a catcher becomes deactivated by time, the player loses streak
	- Catchers constantly check in front
	- 
*/

//#define DRUM_CATCHER_ACTIVE_TIME 0.05f
//#define DRUM_NOTE_HITTING_TIMER 0.12f
//#define DRUM_STAR_ACTIVATION_TIMER DRUM_NOTE_HITTING_TIMER

namespace Rhythmic
{
	constexpr int CONST_DRUMS_SP_ACTIVATE_TRIED = 1;
	constexpr int CONST_DRUMS_SP_ACTIVATE_ACTIVATED = 2;
	constexpr int CONST_DRUMS_SP_ACTIVATE_NO_CHANGE = 0;

	constexpr float CONST_DRUM_SP_ACTIVATE_TIMER = 0.06f;

	class InstrumentDrums : public Instrument
	{
	public:
		InstrumentDrums();
		~InstrumentDrums();

		void UpdatePlayer(const double &timeOffset) override;
		void UpdateTimers() override;
		void PlayerInput(InputEventData *data) override;
		void UpdateBot(const double &timeOffset) override;
		bool HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset = false, bool isStrumAhead = false) override;
		InstrumentCanHitDetail CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime = false, bool keepExtendedSustains = false) override;
		void FixActiveCatchers(const float &delta, const double &timeOffset, int active) override;
		void CorrectInputValues() override;
	private:
		char m_padModifier;
		inline void ActivateHitNote(float time, int catcher, int note)
		{
			m_catcherVisualActiveTimer[catcher] = time;
			m_notesMayHit |= note;
		}
		void ResetStarpowerConditions();

		float m_drumCatcherActiveTime;				// How long the catchers stay active for (visual thing)
		float m_drumChordHitTime;							// The time window of which a whole chord must be hit in, otherwise the combo breaks
		float m_drumStarpowerActivationTime;	// The time window of activating SP

		float m_spActivationTimer;						// The timer to allow SP activation to succeed
		int m_spActivationNoteCount;					// Note count for SP activation. Must be < 2 for activation to succeed
		int m_spActivationLastPad;						// Keep track of last pad hit
		bool m_spActivationForceStreakLoss;		// If enabled, this will force a streak loss if player fails to activate starpower under certain conditions

		/*
		Establishes a timer for a newly hit note. If the timer passes a certain time limit,
		player will lose combo. This is an attempt to make the player hit the notes at
		roughly the same time, and not half a second apart
		*/
		std::pair<int,float> m_noteIndexTimer;

		float	m_catcherVisualActiveTimer[5];

		// Other instruments may use CatcherManager to determine if they can hit notes or not
		// Drums are a special case, since they need to stay active for players to see. But they will be active for a little bit,
		// which means they'll be able to hit any note as long as it's active. Drums MUST only have it active for at least 1 frame.
		// Drums are able to hit any note of the fretboard regardless if the notes are apart of a "chord", which is why notes have
		// a NOTE_FLAG_HIT bit for the flags
		// This value is a bit field
		// Contains what pads were hit last
		int		m_notesMayHit;
	};
}

#endif

