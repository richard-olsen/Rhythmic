#ifndef INSTRUMENT_CALIBRATE_H_
#define INSTRUMENT_CALIBRATE_H_

#include "instrument.hpp"

namespace Rhythmic
{
	class InstrumentCalibrate : public Instrument
	{
	public:
		InstrumentCalibrate();
		~InstrumentCalibrate();

		void UpdatePlayer(const double &timeOffset) override;
		void UpdateTimers() override;
		void PlayerInput(InputEventData *data) override;
		void UpdateBot(const double &timeOffset) override;
		bool HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset = false, bool isStrumAhead = false) override;
		InstrumentCanHitDetail CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime = false, bool keepExtendedSustains = false) override;
		void FixActiveCatchers(const float &delta, const double &timeOffset, int active) override;
		void CorrectInputValues() override;

		std::vector<float> &GetTimesNoteHit();

	private:
		std::vector<float> m_timesNoteHit;
	};
}

#endif

