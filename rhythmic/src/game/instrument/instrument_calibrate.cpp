#include "instrument_calibrate.hpp"

#include "../stage/stage.hpp"

namespace Rhythmic
{
	InstrumentCalibrate::InstrumentCalibrate() :
		Instrument(true),
		m_timesNoteHit()
	{}
	InstrumentCalibrate::~InstrumentCalibrate()
	{}

	void InstrumentCalibrate::UpdateTimers() {}

	void InstrumentCalibrate::UpdatePlayer(const double &timeOffset)
	{
	}
	void InstrumentCalibrate::PlayerInput(InputEventData *data)
	{
		if (m_noteIndex >= m_notePool.size())
			return;

		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_KICK) ||
			data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_KICK_ALT) ||
			data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_DOWN) ||
			data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_UP))
		{
			float time = StageSystem::GetInstance()->GetSongTime();
			m_timesNoteHit.push_back(m_notePool[m_noteIndex].time - time);
			Hit(m_noteIndex, 1, INSTRUMENT_CAN_HIT_NOTE_OPEN, time, true, false, false, false, false, true);
		}
	}
	void InstrumentCalibrate::UpdateBot(const double &timeOffset)
	{
	}
	bool InstrumentCalibrate::HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset, bool isStrumAhead) { return false; }
	InstrumentCanHitDetail InstrumentCalibrate::CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime, bool keepExtendedSustains) { return InstrumentCanHitDetail(); }
	void InstrumentCalibrate::FixActiveCatchers(const float &delta, const double &timeOffset, int active) { }

	std::vector<float> &InstrumentCalibrate::GetTimesNoteHit()
	{
		return m_timesNoteHit;
	}

	void InstrumentCalibrate::CorrectInputValues()
	{ }
}