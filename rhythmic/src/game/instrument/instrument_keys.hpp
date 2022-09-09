#ifndef INSTRUMENT_FIVE_FRET_H_
#define INSTRUMENT_FIVE_FRET_H_

namespace Rhythmic
{
	constexpr float CONST_HOPO_FRONT_END_DEFAULT = 0.05f;

	class InstrumentKeys : public Instrument
	{
	public:
		InstrumentKeys();
		~InstrumentKeys();

		/*
		A function for the player
		*/
		virtual void UpdatePlayer(const double &timeOffset);

		/*
		A function for the bot
		*/
		virtual void UpdateBot(const double &timeOffset);

		/*
		A function that determins the notes and calls hit.

		Will determin if the note needs a strumming, controls m_playerInCombo

		returns true if the note has been hit
		*/
		virtual bool HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset = false, bool isStrumAhead = false);

		/*
		Determins if the catchers can indeed hit a specific note
		*/
		virtual InstrumentCanHitDetail CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime = false, bool keepExtendedSustains = false);

		/*
		Does nothing
		*/
		virtual void FixActiveCatchers(const float &delta, const double &timeOffset, int active);
	private:
		float m_hopoFrontEndTimer; // Controls how far back the player can hold down the fret and still be able to get the hopo/tap
	};

#endif

