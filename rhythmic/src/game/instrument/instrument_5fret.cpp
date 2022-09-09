#include "instrument_5fret.hpp"

#include "../../container/player_container.hpp"

#include <wge/core/logging.hpp>

#include "../engine_vals.hpp"

#include "../../container/engine/engine_container.hpp"

#include "../stage/stage.hpp"

#include "../../util/dear_imgui/imgui.h"

#include <wge/core/time.hpp>

namespace Rhythmic
{

	EventListener listener;
	Instrument5Fret::Instrument5Fret() :
		Instrument(false),
		m_antiGhosting(EngineContainer::GetCurrentEngine()->m_fret5.m_antiGhost),
		m_hopoFrontEndTimer(INSTRUMENT_TIMER_INACTIVE),
		m_hopoFrontEnd(EngineContainer::GetCurrentEngine()->m_fret5.m_infiniteHOPOFrontEnd ? FLT_MAX : ((float)EngineContainer::GetCurrentEngine()->m_fret5.m_hopoFrontEnd * 0.001f)),
		m_strumLeniencyTimer(INSTRUMENT_TIMER_INACTIVE),
		m_hopoLeniencyTimer(INSTRUMENT_TIMER_INACTIVE),
		m_strumLeniency((float)EngineContainer::GetCurrentEngine()->m_fret5.m_strumLeniency * 0.001f),
		m_hopoLeniency((float)EngineContainer::GetCurrentEngine()->m_fret5.m_hopoLeniency * 0.001f),
		m_tapHopoOverstrumProtection((float)EngineContainer::GetCurrentEngine()->m_fret5.m_timeTapIgnoreOverstrum * 0.001f)
	{
	}
	Instrument5Fret::~Instrument5Fret()
	{
	}

	void Instrument5Fret::ImGuiDEBUG(void*)
	{
	}

	void Instrument5Fret::UpdateTimers()
	{
		float &timeStep = WGE::Core::TimeStep.m_timeStep;

		if (m_hopoFrontEndTimer > INSTRUMENT_TIMER_INACTIVE)
			m_hopoFrontEndTimer -= timeStep;
		
		if (m_strumLeniencyTimer > INSTRUMENT_TIMER_INACTIVE)
			m_strumLeniencyTimer -= timeStep;

		if (m_hopoLeniencyTimer > INSTRUMENT_TIMER_INACTIVE)
			m_hopoLeniencyTimer -= timeStep;
	}

	void Instrument5Fret::UpdatePlayer(const double &timeOffset)
	{
		if (m_parentContainer->m_settings.noStrum && !m_scoreDisqualifier.guitar.noStrum)
			m_scoreDisqualifier.guitar.noStrum = true;

		// Adjust some timers

		if (TimerExpired(m_hopoFrontEndTimer))
			m_playerChangedCatchersSinceLastHit = false;

		float whammy = InputGetDevice(m_parentContainer->m_inputHandle)->m_lastInputEvent.axis[INPUT_BUTTON_WHAMMY].second;

		if (m_activeSustains.empty())
			m_currentWhammyValue = whammy;
		else
			m_currentWhammyValue = glm::mix(m_currentWhammyValue, whammy, 30 * WGE::Core::TimeStep.m_timeStep);
		
		DoWhammy(0, m_currentWhammyValue);

		if (m_noteIndex < m_notePool.size()) // Attempt to get any hopos/taps within the front end or any note within the strum leniency
			HitHelper(m_noteIndex, GetChordEndingIndex(m_notePool, m_noteIndex), false, timeOffset);
	}

	void Instrument5Fret::PlayerInput(InputEventData *data)
	{
		// process input

		float timeOffset = StageSystem::GetInstance()->GetSongTime();

		bool doCatcherActivation = false;
		int catcherPressed = 0;

		// Green
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_GREEN))
		{
			doCatcherActivation = true;
			catcherPressed++;
			m_catcherManager.SetCatchersActivationStatus(0, true);
		}
		else if (data->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_GREEN))
		{
			doCatcherActivation = true;
			m_catcherManager.SetCatchersActivationStatus(0, false);
		}
		// Red
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_RED))
		{
			doCatcherActivation = true;
			catcherPressed++;
			m_catcherManager.SetCatchersActivationStatus(1, true);
		}
		else if (data->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_RED))
		{
			doCatcherActivation = true;
			m_catcherManager.SetCatchersActivationStatus(1, false);
		}
		// Yellow
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_YELLOW))
		{
			doCatcherActivation = true;
			catcherPressed++;
			m_catcherManager.SetCatchersActivationStatus(2, true);
		}
		else if (data->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_YELLOW))
		{
			doCatcherActivation = true;
			m_catcherManager.SetCatchersActivationStatus(2, false);
		}
		// Blue
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_BLUE))
		{
			doCatcherActivation = true;
			catcherPressed++;
			m_catcherManager.SetCatchersActivationStatus(3, true);
		}
		else if (data->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_BLUE))
		{
			doCatcherActivation = true;
			m_catcherManager.SetCatchersActivationStatus(3, false);
		}
		// Orange
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_ORANGE))
		{
			doCatcherActivation = true;
			catcherPressed++;
			m_catcherManager.SetCatchersActivationStatus(4, true);
		}
		else if (data->m_buttonsReleased & ButtonToFlag(INPUT_BUTTON_ORANGE))
		{
			doCatcherActivation = true;
			m_catcherManager.SetCatchersActivationStatus(4, false);
		}

		if (doCatcherActivation)
		{
			m_hopoFrontEndTimer = m_hopoFrontEnd;
			m_playerChangedCatchersSinceLastHit = true;
			if (m_noteIndex < m_notePool.size()) // Allow to hit hopo/taps
			{
				int chord = GetChordEndingIndex(m_notePool, m_noteIndex);
				bool hit = HitHelper(m_noteIndex, chord, false, timeOffset);
				if (!hit && m_antiGhosting && m_noteStreak > 0 && catcherPressed > 0 && !CanCatchersHitNote(m_noteIndex, chord, 0, true).canCatchersHit)
					m_ghostCount += catcherPressed;
			}
		}

		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_UP) || data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_DOWN))
		{
			if (TimerExpired(m_strumLeniencyTimer, false))
				ResetStreak(timeOffset, true);

			if (m_noteIndex < m_notePool.size())
			{
				m_strumLeniencyTimer = m_strumLeniency;

				HitHelper(m_noteIndex, GetChordEndingIndex(m_notePool, m_noteIndex), true, timeOffset);
			}
		}

		if (data->axis[INPUT_BUTTON_STARPOWER].second == 1.0f || data->m_buttonsPressed & (1 << INPUT_BUTTON_STARPOWER))
			ActivateStarpower();

		if (StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode())
		{
			if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_SELECT))
			{
				StageSystem::GetInstance()->Reset();
			}
			if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_LEFT)) // Slow down
			{
				float speed = StageSystem::GetInstance()->GetPlaybackSpeed();
				speed *= 100;
				speed = round(speed);
				speed -= 5; // Taking 5%

				// Check if the new speed is bigger or equal to 5
				if (speed >= 5)
					StageSystem::GetInstance()->SetPlaybackSpeed(speed / 100.0f); // -5%
			}
			if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_RIGHT)) // Speed up
			{
				float speed = StageSystem::GetInstance()->GetPlaybackSpeed();
				speed *= 100;
				speed = round(speed);
				speed += 5; // Adding 5%

				// Check if the new speed is less than or equal to 5000
				if (speed <= 5000)
					StageSystem::GetInstance()->SetPlaybackSpeed(speed / 100.0f); // +5%
			}
		}
	}

	void Instrument5Fret::UpdateBot(const double &timeOffset)
	{
		if (m_noteIndex >= m_notePool.size())
			return;

		Note note = m_notePool[m_noteIndex];
		if (note.time - timeOffset < 0)
		{
			int activeCatcher = 0;

			if (m_extendedSustainMode)
			{
				// We need to get the currently active sustains for extended sustain visuals with the bot
				for (unsigned int i = 0; i < m_activeSustains.size(); i++)
				{
					activeCatcher |= (1 << m_notePool[m_activeSustains[i]].note);
				}
			}

			unsigned int noteAmount = GetChordEndingIndex(m_notePool, m_noteIndex);

			for (unsigned int i = m_noteIndex; i < (m_noteIndex + noteAmount); i++)
				activeCatcher |= (1 << m_notePool[i].note);

			m_catcherManager.SetActiveCatchers(activeCatcher);

			m_strumLeniencyTimer = timeOffset + m_strumLeniency;
			HitHelper(m_noteIndex, noteAmount, true, timeOffset);
		}
	}

	bool Instrument5Fret::HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset, bool isStrumAhead)
	{
		if (pointer >= m_notePool.size() || (pointer + removeAmount) > m_notePool.size())
			return false; // Return false if the pointer goes beyond the amount of notes, or if it thinks it's a chord and it's accessing notes outside the bounds of notepool

		bool inRange = IsNoteInRange(pointer, timeOffset);

		if (!inRange)
		{
			// Player strummed, and the leniency timer expired. Ignore if player has no strum enabled
			if (TimerExpired(m_strumLeniencyTimer) && !m_parentContainer->m_settings.noStrum)
			{
				// For the taps/hopos
				// In this case, the tap/hopo will use the strum since there isn't any notes in range
				if (timeOffset <= (m_lastTapHitTime + m_tapHopoOverstrumProtection))
				{
					m_lastTapHitTime = -1;
				}
				else
				{
					m_extendedSustainMode = false;

					for (auto it = m_activeSustains.begin(); it != m_activeSustains.end(); it++)
					{
						m_missedSustains.push_back({ m_notePool[*it].lastScoreCheck, *it });
					}
					m_activeSustains.clear();

					ResetStreak(timeOffset, true);
				}
				m_strumLeniencyTimer = INSTRUMENT_TIMER_INACTIVE;
				m_hopoLeniencyTimer = INSTRUMENT_TIMER_INACTIVE;
			}

			return false;
		}

		Note &currentNote = m_notePool[pointer];

		bool isHopo = IS_NOTE(currentNote, NOTE_FLAG_HOPO);
		bool isTap = IS_NOTE(currentNote, NOTE_FLAG_TAP);

		InstrumentCanHitDetail detail = CanCatchersHitNote(pointer, removeAmount, timeOffset, true);

		float songSpeed = StageSystem::GetInstance()->GetPlaybackSpeed();
		
		if (m_playerChangedCatchersSinceLastHit || (currentNote.note == NOTE_OPEN && m_noteStreak > 0))
		{
			if ((isHopo || isTap) && ((m_noteStreak > 0 && !isStrumAhead) || // A player must have a note streak going in order to hit hopos
				(StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode() && m_noteIndex == 0))) // If practice mode the first HOPO doesn't require a strum
			{
				float usefulTime = currentNote.time - timeOffset;
				char hit = 0;

				if (detail.canCatchersHit && m_ghostCount < (removeAmount + 1))
				{
					m_hopoLeniencyTimer = m_hopoLeniency;
					if (usefulTime <= ToCorrectRange(m_rangeHOPOFront, songSpeed))
						hit = 1;
				}
				else
				{
					if (m_hopoLeniencyTimer > 0)
					{
						if ((usefulTime - m_hopoLeniencyTimer) < ToCorrectRange(m_rangeHOPOFront, songSpeed))
							hit = (1 | (1 << 1));
					}
				}

				if (hit)
				{
					if (hit & (1 << 1))
						Hit(pointer, removeAmount, GetNotesToHit(m_notePool, pointer, removeAmount), timeOffset);
					else
						Hit(pointer, removeAmount, detail.notesToHit, timeOffset);
					m_ghostCount = 0;
					m_lastTapHitTime = timeOffset;
					m_playerChangedCatchersSinceLastHit = (hit & (1 << 1)) > 0;
					m_hopoLeniencyTimer = 0;
					return true;
				}
			}
			else if (isTap && m_noteStreak == 0)
			{
				float usefulTime = currentNote.time - timeOffset;

				if (!detail.canCatchersHit) // Try for a strum ahead if the player has no streak going on
				{
					int note = pointer + removeAmount;
					int chord = GetChordEndingIndex(m_notePool, note);
					return HitHelper(note, chord, strummed, timeOffset, true, true);
				}
				else if (detail.canCatchersHit && usefulTime <= ToCorrectRange(m_rangeHOPOFront, songSpeed))
				{
					// Don't bother with hopo leniency if the player isn't in a streak
					Hit(pointer, removeAmount, detail.notesToHit, timeOffset);
					m_ghostCount = 0;
					m_lastTapHitTime = timeOffset;
					m_playerChangedCatchersSinceLastHit = false;
					m_hopoLeniencyTimer = 0;
					return true;
				}
			}
		}

		if (!inRange)
			return false;

		if (m_strumLeniencyTimer > 0)
		{
			if (detail.canCatchersHit)
			{
				Hit(pointer, removeAmount, detail.notesToHit, timeOffset);
				m_ghostCount = 0;
				m_lastStrumHitTime = timeOffset;
				m_strumLeniencyTimer = INSTRUMENT_TIMER_INACTIVE;
				m_hopoLeniencyTimer = INSTRUMENT_TIMER_INACTIVE;
				if (m_noteIndex < m_notePool.size()) // Don't bother if the note index is greater than the amount in the note pool
				{
					// Should work regaurdless if it's a chord or not
					// But this shouldn't function of the note IS a hopo/tap
					if (!isTap && !isHopo)
					{
						m_playerChangedCatchersSinceLastHit = true;
						m_hopoFrontEndTimer = FLT_MAX; // Always use infinite front end after strumming. This timer will end up getting reset if the player changes the frets
					}
				}
				return true;
			}
			else if (m_noteStreak == 0)
			{
				int note = pointer + removeAmount;
				int chord = GetChordEndingIndex(m_notePool, note);
				return HitHelper(note, chord, strummed, timeOffset, true, true);
			}
		}

		return false;
	}

	InstrumentCanHitDetail Instrument5Fret::CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime, bool keepExtendedSustains)
	{
		InstrumentCanHitDetail detail = { 0 };

		if (!ignoreTime)
			if (!IsNoteInRange(pointer, timeOffset))
				return detail;

		Note note = m_notePool[pointer];

		int activeCatchers = m_catcherManager.GetCatchersActive();

		// First test the open notes since there are not supposed to be any notes held down to hit one
		if (note.note == NOTE_OPEN && activeCatchers == 0)
		{
			detail.canCatchersHit = true;
			detail.notesToHit = INSTRUMENT_CAN_HIT_NOTE_OPEN;
			return detail;
		}

		// Remove any frets held down that are used to hit the active sustains (should only be done when extended sustains are active)
		if (m_extendedSustainMode && !keepExtendedSustains)
			for (int i = 0; i < m_activeSustains.size(); i++)
				activeCatchers &= ~(1 << m_notePool[m_activeSustains[i]].note);

		// Player may not anchor if the note is a double and the double is a strum note
		bool allowAnchor = !(IS_NOTE(note, NOTE_FLAG_IS_CHORD) && IS_NOTE(note, NOTE_FLAG_STRUM));

		if (allowAnchor)
		{
			if (IS_NOTE(note, NOTE_FLAG_IS_CHORD))
			{
				for (int i = pointer; i < pointer + removeAmount; i++)
				{
					int noteBit = 1 << m_notePool[i].note;
					if (!(activeCatchers & noteBit))
						return detail;
					activeCatchers &= ~(noteBit);
					detail.notesToHit |= noteBit;
				}

				detail.canCatchersHit = (activeCatchers < (1 << (note.note + 1)));
				return detail;
			}
			int noteBit = 1 << note.note;
			detail.notesToHit = noteBit;
			detail.canCatchersHit = activeCatchers < (1 << (note.note + 1)) && (activeCatchers & noteBit);
			return detail;
		}

		int targetNote = 0;
		for (int i = pointer; i < (pointer + removeAmount); i++)
		{
			int noteBit = 1 << m_notePool[i].note;
			targetNote |= noteBit;
			detail.notesToHit |= noteBit;
		}

		detail.canCatchersHit = activeCatchers == targetNote;
		return detail;
	}

	void Instrument5Fret::FixActiveCatchers(const float &delta, const double &timeOffset, int active) { }

	void Instrument5Fret::CorrectInputValues()
	{
		InputDevice *device = InputGetDevice(m_parentContainer->m_inputHandle);

		m_catcherManager.SetCatchersActivationStatus(0, BUTTON_HELD(device, INPUT_BUTTON_GREEN));
		m_catcherManager.SetCatchersActivationStatus(1, BUTTON_HELD(device, INPUT_BUTTON_RED));
		m_catcherManager.SetCatchersActivationStatus(2, BUTTON_HELD(device, INPUT_BUTTON_YELLOW));
		m_catcherManager.SetCatchersActivationStatus(3, BUTTON_HELD(device, INPUT_BUTTON_BLUE));
		m_catcherManager.SetCatchersActivationStatus(4, BUTTON_HELD(device, INPUT_BUTTON_ORANGE));
	}
}