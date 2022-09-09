#include "instrument_drum.hpp"

#include "../../container/player_container.hpp"

#include "../player.hpp"

#include "../../util/misc.hpp"

#include "../engine_vals.hpp"

#include "../../container/engine/engine_container.hpp"

#include "../stage/stage.hpp"

namespace Rhythmic
{
	InstrumentDrums::InstrumentDrums() :
		Instrument(false),
		m_padModifier(0),
		m_drumCatcherActiveTime((float)EngineContainer::GetCurrentEngine()->m_drums.m_padActive * 0.001f),
		m_drumChordHitTime((float)EngineContainer::GetCurrentEngine()->m_drums.m_chordTime * 0.001f),
		m_drumStarpowerActivationTime((float)EngineContainer::GetCurrentEngine()->m_drums.m_spActivationTime * 0.001f),
		m_spActivationNoteCount(0),
		m_spActivationTimer(INSTRUMENT_TIMER_INACTIVE),
		m_spActivationLastPad(1 << NOTE_OPEN),
		m_spActivationForceStreakLoss(false)
	{
		float currentTime = Util::GetTime() - 0.26f; // Stops all of the pads being held down at the start
		for (int i = 0; i < 5; i++)
		{
			m_catcherVisualActiveTimer[i] = currentTime;
		}
		m_noteIndexTimer = std::pair<int,float>(-1, currentTime);
	}
	InstrumentDrums::~InstrumentDrums()
	{ }

	void InstrumentDrums::UpdateTimers()
	{

	}
	void InstrumentDrums::UpdatePlayer(const double &timeOffset)
	{
		if (m_parentContainer->m_settings.autoKick && !m_scoreDisqualifier.drums.autoKick)
			m_scoreDisqualifier.drums.autoKick = true;

		if (Util::GetTime() > m_spActivationTimer && m_spActivationTimer > 0)
		{
			if (m_spActivationForceStreakLoss)
				ResetStreak(timeOffset, true);				// Consider it an overstrum(hit), as this will only be enabled if the player didn't hit a note
			ResetStarpowerConditions();
		}

		int activeCatchers = 0;

		m_notesMayHit = 0;

		float currentTime = Util::GetTime();

		m_catcherManager.SetActiveCatchers(activeCatchers);

		if ((bool)m_parentContainer->m_settings.autoKick && m_noteIndex < m_notePool.size() && m_notePool[m_noteIndex].time - timeOffset <= 0)
		{
			int v = GetChordEndingIndex(m_notePool, m_noteIndex);
			for (int i = 0; i < v; i++)
				if (m_notePool[m_noteIndex + i].note == NOTE_OPEN &&
					!IS_NOTE(m_notePool[m_noteIndex + i], NOTE_FLAG_HIT))
					m_notesMayHit |= INSTRUMENT_CAN_HIT_NOTE_OPEN;
		}

		FixActiveCatchers(0, currentTime, 0);

		if (m_notesMayHit > 0 && (m_noteIndex > 0 || !m_notesInRange.empty()))
		{
			int removeAmount = GetChordEndingIndex(m_notePool, m_noteIndex);
			HitHelper(m_noteIndex, removeAmount, false, timeOffset, false, false);
			m_notesMayHit = 0;
		}

		if (m_noteIndex == m_noteIndexTimer.first && timeOffset - m_drumChordHitTime > m_noteIndexTimer.second)
		{
			m_noteIndexTimer = std::pair<int,float>(-1, timeOffset);
			ResetStreak(timeOffset, false);
		}
	}
	void InstrumentDrums::PlayerInput(InputEventData *data)
	{

		volatile bool padMod = data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_MOD_RB_DRUMS_PAD);
		volatile bool cymMod = data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_MOD_RB_DRUMS_CYMBAL);

		// Adjust for timestamp
		float timeOffset = StageSystem::GetInstance()->GetSongTime();
		
		if (padMod && cymMod)
			m_padModifier = 2;
		else if (padMod)
			m_padModifier = 0;
		else if (cymMod)
			m_padModifier = 1;

		m_notesMayHit = 0;

		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_RED)) // Lane 1 is always RED
			ActivateHitNote(data->timestamp, 0, INSTRUMENT_CAN_HIT_NOTE_1);
		//-----------------------------------------------------------------------------------------------------------------------------------
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_YELLOW)) // Lane 2 is always YELLOW
			ActivateHitNote(data->timestamp, 1, m_drumsIsPro && m_padModifier == 1 ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_1  : INSTRUMENT_CAN_HIT_NOTE_2);

		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_CYMBAL_YELLOW))
			ActivateHitNote(data->timestamp, 1, m_drumsIsPro ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_1 : INSTRUMENT_CAN_HIT_NOTE_2);
		//-----------------------------------------------------------------------------------------------------------------------------------
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_BLUE)) // Lane 3 is always BLUE
			ActivateHitNote(data->timestamp, 2, m_drumsIsPro && m_padModifier == 1 ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_2 : INSTRUMENT_CAN_HIT_NOTE_3);

		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_CYMBAL_BLUE))
			ActivateHitNote(data->timestamp, 2, m_drumsIsPro ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_2 : INSTRUMENT_CAN_HIT_NOTE_3);
		//-----------------------------------------------------------------------------------------------------------------------------------
		
		if (data->m_buttonsPressed & ButtonToFlag(m_drumsIs4Lane ? INPUT_BUTTON_GREEN : INPUT_BUTTON_ORANGE)) // Lane 4 is either GREEN or ORANGE
			ActivateHitNote(data->timestamp, 3, m_drumsIsPro && m_padModifier == 1 ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_3 : INSTRUMENT_CAN_HIT_NOTE_4);

		if ((data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_CYMBAL_GREEN)) && m_drumsIs4Lane)
			ActivateHitNote(data->timestamp, 3, m_drumsIsPro ? INSTRUMENT_CAN_HIT_NOTE_CYMBAL_3 : INSTRUMENT_CAN_HIT_NOTE_4);
		//-----------------------------------------------------------------------------------------------------------------------------------
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_GREEN) && !m_drumsIs4Lane) // Lane 5 is always GREEN
			ActivateHitNote(data->timestamp, 4, INSTRUMENT_CAN_HIT_NOTE_5);
		//-----------------------------------------------------------------------------------------------------------------------------------
		if (data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_KICK) || data->m_buttonsPressed & ButtonToFlag(INPUT_BUTTON_KICK_ALT))
			m_notesMayHit |= INSTRUMENT_CAN_HIT_NOTE_OPEN;


		if (m_notesMayHit > 0 && (m_noteIndex > 0 || !m_notesInRange.empty()))
		{
			int removeAmount = GetChordEndingIndex(m_notePool, m_noteIndex);
			bool hit = HitHelper(m_noteIndex, removeAmount, false, timeOffset, false, false);
			m_notesMayHit = 0;

			m_spActivationForceStreakLoss = (m_spActivationLastPad >= 0 && !hit);

			if (!hit && (m_starpowerAmount < 0.5f || m_isStarpowerActive))
				ResetStreak(timeOffset, true);
		}

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
	void InstrumentDrums::UpdateBot(const double &timeOffset)
	{
		float currentTime = Util::GetTime();

		if (m_noteIndex < m_notePool.size())
		{
			Note &notePointer = m_notePool[m_noteIndex];
			m_notesMayHit = 0;

			int removeAmount = GetChordEndingIndex(m_notePool, m_noteIndex);

			if (notePointer.time - timeOffset <= 0)
			{

				for (int i = 0; i < removeAmount; i++)
				{
					int note = m_notePool[m_noteIndex + i].note;

					if (note == 5)
						m_notesMayHit |= INSTRUMENT_CAN_HIT_NOTE_OPEN;
					else
					{
						int noteVal = note;
						if (IS_NOTE(m_notePool[m_noteIndex + i], NOTE_FLAG_CYMBAL) && m_drumsIsPro)
							note += 5;
						ActivateHitNote(currentTime, noteVal, (1 << note));
					}
				}
			}

			if (m_notesMayHit > 0)
				HitHelper(m_noteIndex, removeAmount, false, timeOffset, false, false);
		}

		FixActiveCatchers(0, currentTime, 0);
	}
	bool InstrumentDrums::HitHelper(int index, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset, bool isStrumAhead)
	{
		// Don't hit any notes if the indexes are greater than the note pool size
		if (index >= m_notePool.size() || (index + removeAmount) > m_notePool.size())
			return false;

		InstrumentCanHitDetail hitDetail = CanCatchersHitNote(index, removeAmount, timeOffset);

		bool hitSucceed = false;
		
		if (hitDetail.canCatchersHit)
		{

			/*if (isStrumAhead)
				m_noteIndex = index;*/
			
			float currentTime = Util::GetTime();

			if (m_noteIndexTimer.first != m_noteIndex)
				m_noteIndexTimer = std::pair<int,float>(m_noteIndex, currentTime);
			
			Hit(index, removeAmount, hitDetail.notesToHit, timeOffset, true, true, true, forceStreakReset, true);

			//if (starpowerActivationTried)
			//	std::get<CONST_DRUMS_SP_GH_TUPLE_DISALLOW_MISS>(m_starpowerActivationGHStyle) = true;

			hitSucceed = true;
		}
		else if (!(hitDetail.missFlag & INSTRUMENT_MISS_FLAG_OUT_OF_RANGE))
		{
			int newPointer = index + removeAmount;
			if (newPointer < m_notePool.size())
			{
				int newRemove = GetChordEndingIndex(m_notePool, newPointer);
				hitSucceed = HitHelper(newPointer, newRemove, false, timeOffset, true, true);
			}
		}

		/*
		DO NOT CONTINUE IF HitHelper IS LOOKING AHEAD!
		*/
		if (isStrumAhead)
			return hitSucceed;

		/*
		Starpower activation

		If there isn't a recorded pad, then it's safe to assume the timer
		has yet to start
		*/

		/*
		No reason to continue if player can't even activate starpower to begin with
		*/
		if (m_starpowerAmount < 0.5f && !m_isStarpowerActive)
			return hitSucceed;

		/*
		Ignore kicks

		If kick has been hit with others, get rid of kick
		If only kick has been hit, ignore any starpower activations
		*/
		if (m_notesMayHit == (1 << NOTE_OPEN))
			return hitSucceed;
		m_notesMayHit &= ~(1 << NOTE_OPEN);
		if (hitDetail.notesToHit & (1 << NOTE_OPEN))
		{
			hitDetail.notesToHit &= ~(1 << NOTE_OPEN);
			hitDetail.noteCount--;
		}

		if (hitDetail.noteCount == 2)
			return hitSucceed;


		bool padIsSame = (m_notesMayHit & m_spActivationLastPad) > 0;
		/*
		Reset timers if the pad is the same
		*/
		if (padIsSame)
		{
			bool forceReset = m_spActivationForceStreakLoss;
			ResetStarpowerConditions();

			// Only reset streak if the player didn't hit anything
			if (!hitSucceed)
			{
				if (forceReset)
					ResetStreak(timeOffset, true);
				m_spActivationForceStreakLoss = true;
			}
		}

		/*
		First pad / Dual pad test

		When spActivationLastPad is less than zero,
		it's safe to assume the timer isn't active
		*/
		if (m_spActivationLastPad == (1 << NOTE_OPEN))
		{
			int activationCount = 0;
			int padActive = -1;

			/*
			Count for all the pads hit within the frame. There's a possible
			chance that two pads were hit at the same time. It can happen
			*/
			for (int i = 0; i < (m_drumsIs4Lane ? 4 : 5); i++)
			{
				if (m_notesMayHit & (1 << i))
				{
					activationCount++;
					padActive = (1 << i);
				}
			}

			/*
			Activate just in case
			*/
			if (activationCount >= 2)
			{
				ActivateStarpower();
				hitSucceed = true;
			}
			else
			{
				/*
				Start timer and set a few things to make sure player doesn't immediately lose their combo
				*/
				m_spActivationTimer = Util::GetTime() + CONST_DRUM_SP_ACTIVATE_TIMER;

				m_spActivationLastPad = padActive;

				m_spActivationForceStreakLoss = !hitSucceed;
			}
		}
		else
		{
			/*
			When the second pad is hit to activate starpower, it MUST not be the same pad as before!
			*/

			double activationTime = Util::GetTime();
			bool activation = activationTime < m_spActivationTimer;

			/*
			Don't bother activating starpower at this point
			It's likely the case that the player is hitting notes
			and doesn't want to activate starpower
			
			Otherwise, activate it
			*/
			if (activation && !padIsSame && (!hitSucceed || (hitSucceed && m_spActivationForceStreakLoss)))
			{
				ActivateStarpower();
				hitSucceed = true;
			}

			ResetStarpowerConditions();
		}
		
		return hitSucceed;
	}

	void InstrumentDrums::ResetStarpowerConditions()
	{
		m_spActivationForceStreakLoss = false;
		m_spActivationLastPad = 1 << NOTE_OPEN;
		m_spActivationNoteCount = 0;
		m_spActivationTimer = -20;
	}

	InstrumentCanHitDetail InstrumentDrums::CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime, bool keepExtendedSustains)
	{
		InstrumentCanHitDetail detail = { 0 };

		if (!ignoreTime)
			if (!IsNoteInRange(pointer, timeOffset))
			{
				detail.missFlag |= INSTRUMENT_MISS_FLAG_OUT_OF_RANGE;
				return detail;
			}

		int tryHit = m_notesMayHit;

		detail.canCatchersHit = true; // Always assume true. This function isn't called every frame, only when the drummer hits a pad
		
		for (int i = 0; i < removeAmount; i++)
		{
			Note &currentNote = m_notePool[pointer + i];
			bool cymbal = (m_drumsIsPro && m_padModifier != 2) ? IS_NOTE(currentNote, NOTE_FLAG_CYMBAL) : false;
			int noteVal = 1 << currentNote.note;
			int compareVal = 1 << (currentNote.note + (cymbal ? 5 : 0));
			
			detail.allNotes |= noteVal;

			if (tryHit & compareVal)
			{
				if (!IS_NOTE(m_notePool[pointer + i], NOTE_FLAG_HIT))
				{
					detail.notesToHit |= noteVal;
					detail.noteCount++;
				}
				else
					detail.canCatchersHit = false;
			}
			
			tryHit &= ~compareVal;
		}
		if (tryHit > 0)
			detail.canCatchersHit = false;

		return detail;
	}
	void InstrumentDrums::FixActiveCatchers(const float &delta, const double &timeOffset, int active)
	{
		int activeCatchers = 0;
		for (int i = 0; i < (m_drumsIs4Lane ? 4 : 5); i++)
			if (timeOffset - m_catcherVisualActiveTimer[i] <= m_drumCatcherActiveTime)
				activeCatchers |= (1 << i);
		m_catcherManager.SetActiveCatchers(activeCatchers);
	}
	void InstrumentDrums::CorrectInputValues()
	{
		m_notesMayHit = 0;
	}
}