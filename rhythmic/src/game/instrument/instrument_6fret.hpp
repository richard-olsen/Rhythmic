#ifndef INSTRUMENT_SIX_FRET_H_
#define INSTRUMENT_SIX_FRET_H_

#include "instrument.hpp"

#include "../../events/event_system.hpp"
#include "../../events/events_common.hpp"

namespace Rhythmic
{

	class Instrument6Fret : public Instrument
	{
	public:
		Instrument6Fret();
		~Instrument6Fret();

		void ImGuiDEBUG(void*);

		void UpdatePlayer(const double& timeOffset) override;
		void UpdateTimers() override;
		void PlayerInput(InputEventData* data) override;
		void UpdateBot(const double& timeOffset) override;
		bool HitHelper(int pointer, int removeAmount, bool strummed, const double& timeOffset, bool forceStreakReset = false, bool isStrumAhead = false) override;
		InstrumentCanHitDetail CanCatchersHitNote(int pointer, int removeAmount, const double& timeOffset, bool ignoreTime = false, bool keepExtendedSustains = false) override;
		void CorrectInputValues() override;
		/*
		Does nothing
		*/
		virtual void FixActiveCatchers(const float& delta, const double& timeOffset, int active);
	private:
		int m_ghostingCount;		// Ghost count, so players can't cheese the song
		bool m_antiGhosting;		// Anti-ghosting feature

		float m_hopoFrontEndTimer; // Controls how far back the player can hold down the fret and still be able to get the hopo/tap
		float m_hopoFrontEnd;		// Store the value of the front end

		float m_strumLeniencyTimer; // Controls the strum leniency
		float m_strumLeniency;		// Store the value of the strum leniency

		float m_hopoLeniencyTimer; // Controls the hopo/tap leniency
		float m_hopoLeniency;		// Store the value of the hopo/tap leniency

		float m_tapHopoOverstrumProtection;	// Combo protection for if the player strums on a hopo/tap
	};
}

#endif

