#include "instrument.hpp"

#include "../stage/stage.hpp"
#include "../player.hpp"
#include "../../container/player_container.hpp"

#include "../../util/settings.hpp"
#include "../../util/misc.hpp"

#include "../../io/io_window.hpp"
#include <wge/core/logging.hpp>

#include "../engine_vals.hpp"
#include "../modifiers.hpp"

#include "../../container/engine/engine_container.hpp"

#include <iostream>
#include <algorithm>

namespace Rhythmic
{
	Instrument::Instrument(bool ignoreRange) :
		m_holdingPool(),
		m_notePool(),
		m_catcherManager(),
		m_noteIndex(0),
		m_noteStreak(0),
		m_highestNoteStreak(0),
		m_sustainDropLeniencyTimer(0),
		m_ghostCount(0),
		m_playerChangedCatchersSinceLastHit(true),
		m_extendedSustainMode(false),
		m_lastTapHitTime(-1),
		m_lastStrumHitTime(-1),
		m_playerScore(0),
		m_ignoreRange(ignoreRange),
		m_baseScore(0),
		m_chart(0),
		m_created(false),
		m_lastWhammyValue(0),
		m_currentWhammyValue(0)
	{
	}

	Instrument::~Instrument()
	{
		Destroy();
		m_created = false;
	}

	void Instrument::Create(ContainerPlayer *player, float videoCalibration)
	{
		if (!m_inputListener.wasRemoved)
			EventRemoveListener(m_inputListener);
		m_inputListener = EventAddListener(ET_INPUT, CREATE_MEMBER_CB(Instrument::_OnInput));

		m_isBot = player->m_settings.useBot;
		m_parentContainer = player;
		m_isFC = true;
		
		m_notesHit = 0;
		m_notesMissed = 0;

		m_noteIndex = 0;
		m_noteStreak = 0;
		m_highestNoteStreak = 0;
		m_playerChangedCatchersSinceLastHit = true;
		m_extendedSustainMode = false;
		m_lastTapHitTime = -1;
		m_lastStrumHitTime = -1;
		m_playerScore = 0;
		m_timerWhammy = 0;

		m_missedSustains.clear();

		// Set video calibration
		SettingValue value;
		value.type = SETTING_GAME_CALIBRATION_VIDEO;
		SettingsGet(&value);
		m_videoCalibration = (float)value.m_iValue * 0.001f;

		// Use the stage to access the chart and get a note pool
		m_chart = StageSystem::GetInstance()->GetChart();

		m_drumsIs4Lane = (bool)player->m_settings.fourLaneDrums;
		m_drumsIsPro = player->m_drumsUseCymbals;

		m_sections.clear();
		m_sections.resize(m_chart->listSection.size());

		// Calculate starpower
		m_starpowerAmount = 0;
		m_starpowerPhrases = m_chart->spPhrases[player->m_instrument][player->m_difficulty];
		GetCorrectedStarpowerPhrases(m_starpowerPhrases, *m_chart);
		m_isStarpowerActive = false;

		// Retrieve all notes, events, sections, etc.
		ChartToPool ctop;
		ctop.notes = &m_holdingPool;
		ctop.sections = &m_sections;
		ctop.starpowerPhrases = &m_starpowerPhrases;

		if (player->m_instrument == -1) // Trick the loader into thinking it's loading the guitar track
			ctop.instrument = INSTRUMENT_TYPE_GUITAR;
		else
			ctop.instrument = player->m_instrument;

		ctop.difficulty = player->m_difficulty;
		ctop.asFrets = ((player->m_instrument == INSTRUMENT_TYPE_DRUMS) && m_drumsIs4Lane) ? 4 : 5;
		//6 fret
		if (player->m_instrument == INSTRUMENT_TYPE_6FRET || player->m_instrument == INSTRUMENT_TYPE_6FRETBASS) {
			ctop.asFrets = 6;
		}

		ctop.drumCymbalsLoad = (m_drumsIs4Lane && m_drumsIsPro) ?
			((player->m_settings.useCymbal1 ? (1 << NOTE_2) : 0) |
			 (player->m_settings.useCymbal2 ? (1 << NOTE_3) : 0) |
			 (player->m_settings.useCymbal3 ? (1 << NOTE_4) : 0))
			:
			0;

		m_maxNotes = ChartCalculate(*m_chart, ctop);
		m_baseScore = ctop.m_baseScore;

		// Apply modifiers
		ApplyModifiers(this, player->m_modifiers, m_drumsIs4Lane);

		// Copy over the notes
		m_notePool = m_holdingPool;

		m_scoreDisqualifier.drums.autoKick = false;
		m_scoreDisqualifier.guitar.noStrum = false;

		//score multiplier, bass can go up to 6
		if (player->m_instrument == INSTRUMENT_TYPE_BASS || player->m_instrument == INSTRUMENT_TYPE_6FRETBASS)
			m_maxModifier = 6;
		else {
			m_maxModifier = 4;
		}
			
		//creates catcher manager for different instruments
		if (player->m_instrument == INSTRUMENT_TYPE_DRUMS)
		{
			m_catcherManager.Create(true, false, m_drumsIs4Lane ? 4 : 5);//settings->fourPadDrum ? 4 : 5);
		}
		else if (player->m_instrument == INSTRUMENT_TYPE_6FRET || player->m_instrument == INSTRUMENT_TYPE_6FRETBASS) {
			m_catcherManager.Create(false, true, 6);
		}
		else
		{
			m_catcherManager.Create(false, false, 5);
		}



		float songSpeed = (float)GamePlayVariables::g_chartModifiers.m_songSpeed * 0.01f;
		if (StageSystem::GetInstance()->GetModulePractice()->IsPracticeMode()) songSpeed = 1.0f;

		if (player->m_instrument != INSTRUMENT_TYPE_DRUMS)
		{
			m_rangeFront = (float)EngineContainer::GetCurrentEngine()->m_fret5.m_strumRange * 0.5f * 0.001f;
			m_rangeBack = -(float)EngineContainer::GetCurrentEngine()->m_fret5.m_strumRange * 0.5f * 0.001f;

			m_rangeHOPOFront = (float)EngineContainer::GetCurrentEngine()->m_fret5.m_hopoTapRange * 0.001f;
		}
		else
		{
			m_rangeFront = (float)EngineContainer::GetCurrentEngine()->m_drums.m_rangeCatcher * 0.5f * 0.001f;
			m_rangeBack = -(float)EngineContainer::GetCurrentEngine()->m_drums.m_rangeCatcher * 0.5f * 0.001f;
			m_rangeHOPOFront = m_rangeFront;// (float)EngineContainer::GetCurrentEngine()->m_drums.m_rangeCatcher * 0.001f - videoCalibration;
		}

		std::vector<AnimFrame> framesStar;
		framesStar.push_back({ 25, 13, 2, 2 });
		framesStar.push_back({ 27, 13, 2, 2 });
		framesStar.push_back({ 29, 13, 2, 2 });
		framesStar.push_back({ 31, 13, 2, 2 });
		framesStar.push_back({ 33, 13, 2, 2 });

		// New flames start at 26

		std::vector<AnimFrame> framesHit;
		/*framesHit.push_back({ 25, 16, 2, 5 });
		framesHit.push_back({ 27, 16, 2, 5 });
		framesHit.push_back({ 29, 16, 2, 5 });
		framesHit.push_back({ 31, 16, 2, 5 });*/
		framesHit.push_back({ 26, 16, 2, 3 });
		framesHit.push_back({ 28, 16, 2, 3 });
		framesHit.push_back({ 30, 16, 2, 3 });
		framesHit.push_back({ 32, 16, 2, 3 });
		framesHit.push_back({ 34, 16, 2, 3 });
		framesHit.push_back({ 36, 16, 2, 3 });


		std::vector<AnimFrame> framesHitOpen;
		framesHitOpen.push_back({ 23, 22, 16, 1 });
		framesHitOpen.push_back({ 23, 23, 16, 1 });
		framesHitOpen.push_back({ 23, 24, 16, 1 });
		framesHitOpen.push_back({ 23, 25, 16, 1 });

		for (int i = 0; i < 6; i++)
		{
			m_animStarHit[i].SetFPS(24.0f);
			m_animStarHit[i].SetFrames(framesStar);
			m_animStarHit[i].Play(ANIM_CONTROL_STOP_ON_END);
			m_animStarHit[i].SetFrameIndex(4);			// Set it to the last frame
			m_animStarHit[i].SetSheet(RenderManager::GetSheet("highway_elements"));

			m_animHit[i].SetFPS(30.0f);
			m_animHit[i].SetFrames(framesHit);
			m_animHit[i].Play(ANIM_CONTROL_STOP_ON_END);
			m_animHit[i].SetFrameIndex(3);
			m_animHit[i].SetSheet(RenderManager::GetSheet("highway_elements"));
		}

		m_animHit[6].SetFPS(36.0f);
		m_animHit[6].SetFrames(framesHitOpen);
		m_animHit[6].Play(ANIM_CONTROL_STOP_ON_END);
		m_animHit[6].SetFrameIndex(3);
		m_animHit[6].SetSheet(RenderManager::GetSheet("highway_elements"));

		std::reverse(framesHitOpen.begin(), (framesHitOpen.begin() + 2));

		m_animStarHit[6].SetFPS(24.0f);
		m_animStarHit[6].SetFrames(framesHitOpen);
		m_animStarHit[6].Play(ANIM_CONTROL_STOP_ON_END);
		m_animStarHit[6].SetFrameIndex(3);
		m_animStarHit[6].SetSheet(RenderManager::GetSheet("highway_elements"));

		//m_offset = glm::vec2(0, 0);
		//m_offsetEffect = 0;
		//m_offsetEffectStep = 0;

		m_created = true;
	}

	void Instrument::Destroy()
	{
		m_created = false;

		m_catcherManager.Destroy();
		//m_notePool.Deallocate();
		m_notePool.clear();
		//m_notesInRange.Deallocate();
		m_notesInRange.clear();

		EventRemoveListener(m_inputListener);
	}

	void Instrument::Reset()
	{
		// When resetting, the instrument needs to keep in mind to
		// copy ONLY what is within the stage practice module timing
		// This is to comply with practice mode which will call this
		// function before even beginning a section

		StageSystem *stageSystem = StageSystem::GetInstance();

		int startTime = stageSystem->GetModulePractice()->GetPositionStartOffsetted();
		int endTime = stageSystem->GetModulePractice()->GetPositionEndOffsetted();

		m_notePool.clear();

		// Reset video calibration
		SettingValue value;
		value.type = SETTING_GAME_CALIBRATION_VIDEO;
		SettingsGet(&value);
		m_videoCalibration = (float)value.m_iValue * 0.001f;


		int lastPosition = -1;
		m_noteCount = 0; // May be useful????

		for (size_t i = 0; i < m_holdingPool.size(); i++)
		{
			const Note &current = m_holdingPool[i];

			if (current.chartPos < startTime || current.chartPos >= endTime) // The current note must be within the time
				continue;

			if (current.chartPos != lastPosition)
			{
				lastPosition = current.chartPos;
				m_noteCount++;
			}
			else if (GetInstrumentType() == INSTRUMENT_TYPE_DRUMS)
			{
				lastPosition = current.chartPos;
				m_noteCount++;
			}

			m_notePool.push_back(current);
		}

		// Reset other values such as score, streak, highest streak, fc, etc.

		for (Section &section : m_sections)
		{
			section.notesHitInSection = 0;
		}

		// Look through and organize later, I believe all the important ones are reset
		m_isFC = true;
		m_noteStreak = 0;
		m_highestNoteStreak = 0;
		m_notesHit = 0;
		m_notesMissed = 0;
		m_noteIndex = 0;
		m_lastWhammyValue = 0;
		m_lastTapHitTime = -3;
		m_lastStrumHitTime = -3;
		m_starpowerPhrasesHit = 0;
		m_timerWhammy = 0;
		m_playerInCombo = true;
		m_playerScore = 0;
		m_playerChangedCatchersSinceLastHit = false;
		m_notesInRange.clear();
		m_missedSustains.clear();
		m_isStarpowerActive = false;
		m_extendedSustainMode = false;
		m_activeSustains.clear();
		m_starpowerAmount = 0;
		m_ghostCount = 0;
	}


	void Instrument::Update(const float &delta, const double &timeOffset)
	{
		if (StageSystem::GetInstance()->IsPlaying())
		{
			// Animate the animators
			for (int i = 0; i < 7; i++)
			{
				m_animStarHit[i].Animate();
				m_animHit[i].Animate();
			}
			// Animate the effects
			m_effectBounce.Animate(delta);
			m_effectWobble.Animate(delta);

			m_catcherManager.Update(delta);
			float pauseTime = 0;// m_stage->GetTimePaused();

			UpdateTimers();

			if (pauseTime <= timeOffset && m_parentContainer->m_instrument >= 0)
			{
				UpdateGameplay(delta, timeOffset);
				FixActiveCatchers(delta, timeOffset, m_catcherManager.GetCatchersActive());
			}
			if (m_isBot)
				UpdateBot(timeOffset);
			else
				UpdatePlayer(timeOffset);

			UpdateStarpower(delta, timeOffset);

			// Just to cap the value
			if (m_starpowerAmount > 1)
				m_starpowerAmount = 1;
			if (m_starpowerAmount < 0)
				m_starpowerAmount = 0;
		}
	}


	NotePool *Instrument::GetNotes() { return &m_notePool; }
	NotePool *Instrument::GetNotePoolTemplate() { return &m_holdingPool; }
	std::vector<Section> *Instrument::GetSections() { return &m_sections; }
	CatcherManager *Instrument::GetCatcherManager() { return &m_catcherManager; }

	float lastTime = 0;

	void Instrument::UpdateGameplay(const float &delta, const double &timeOffset)
	{
		if (StageSystem::GetInstance()->GetSongTime() < StageSystem::GetInstance()->GetTimePaused())
			return;

		float timeDifference = Util::GetTime() - lastTime;
		lastTime = Util::GetTime();

		int activeCatchers = m_catcherManager.GetCatchersActive();
		// Get rid of any unnessessary missed sustains
		for (auto it = m_missedSustains.begin(); it != m_missedSustains.end();)
		{
			if (m_notePool[(*it).pointer].sustain <= timeOffset - 2)
			{
				it = m_missedSustains.erase(it);
			}
			else
			{
				it++;
			}
		}

		if (m_activeSustains.size() > 0)
		{
			// Update whammy effect
			
			if (m_effectWhammyData.size() > 0)
			{
				auto it = m_effectWhammyData.begin();
				auto beg = it;
				while (it != m_effectWhammyData.end())
				{
					if (it == beg)
						it->time = 0;
					else
						it->time += delta;
					it++;
				}

				while (!m_effectWhammyData.empty())
				{
					auto back = m_effectWhammyData.back();

					if (back.time >= 7)
						m_effectWhammyData.pop_back();
					else
						break;
				}
			}
		}

		// Get rid of (or hit catchers of) the sustains
		bool addedSustainStarpower = false;
		for (auto it = m_activeSustains.begin(); it != m_activeSustains.end();)
		{
			Note &sustain = m_notePool[*it];

			if (m_extendedSustainMode)
			{
				if (activeCatchers & (1 << sustain.note))
				{
					m_catcherManager.Hit(1 << sustain.note);
					bool remove = AddSustainScore(*it, StageSystem::GetInstance()->GetChartPosition(), timeOffset); // If true, end of sustain is met.

					if (!m_isBot && IS_NOTE(sustain, NOTE_FLAG_IS_STARPOWER))
					{
						if (m_timerWhammy > 0)
						{
							if (!addedSustainStarpower)
							{
								addedSustainStarpower = true;
								
								AddToStarpower(delta, timeOffset);
							}
							SET_NOTE(sustain, NOTE_FLAG_ADDING_STARPOWER);
						}
						else
						{
							CLEAR_NOTE(sustain, NOTE_FLAG_ADDING_STARPOWER);
						}
					}

					if (remove)
						it = m_activeSustains.erase(it);
					else
						it++;
				}
				else
				{
					m_missedSustains.push_back({ m_notePool[*it].lastScoreCheck, *it });
					it = m_activeSustains.erase(it);
				}
			}
			else
			{
				// Get any doubles
				int noteAmount = 0; // Set to zero for now
				//int catchersToHit = 0;

				if (IS_NOTE(sustain, NOTE_FLAG_IS_CHORD)) // The idea of this being the first note in the doubles, and extending the noteAmount to the others
				{
					while ((*it + noteAmount) < m_notePool.size() &&
							m_notePool[*it + noteAmount].time == sustain.time)
					{
						//catchersToHit |= (1 << m_notePool[*it + noteAmount].note);
						noteAmount++;
					}
				}
				else // If there aren't any doubles, then there should be at least one note
				{
					noteAmount++;
					//catchersToHit = (1 << sustain.note);
				}

				InstrumentCanHitDetail detail = CanCatchersHitNote(*it, noteAmount, timeOffset, true, false);

				//if ((sustain.note == NOTE_OPEN && activeCatchers == 0) ||
				//	(CanCatchersHitNote(*it, noteAmount, timeOffset, true).canCatchersHit))
				if (detail.canCatchersHit)
				{
					//if(m_sustainDropLeniencyTimer != 0 && m_activeSustains.size() > 0) // Sustain drop leniency worked!

					m_sustainDropLeniencyTimer = 0;

					bool remove = AddSustainScore(*it, StageSystem::GetInstance()->GetChartPosition(), timeOffset);

					m_catcherManager.Hit(detail.notesToHit);

					int whammy = 0;// m_inputDevice->hitDataAxis[Input::AXIS_WHAMMY].axisValue;

					noteAmount--;

					while (noteAmount >= 0)
					{
						// Add starpower if needed
						Note &current = m_notePool[*it + noteAmount];
						if (!m_isBot && IS_NOTE(current, NOTE_FLAG_IS_STARPOWER))
						{
							if (m_timerWhammy > 0)
							{
								if (!addedSustainStarpower)
								{
									AddToStarpower(delta, timeOffset);
									addedSustainStarpower = true;
								}
								SET_NOTE(current, NOTE_FLAG_ADDING_STARPOWER);
							}
							else
							{
								CLEAR_NOTE(current, NOTE_FLAG_ADDING_STARPOWER);
							}
						}

						noteAmount--;
					}

					if (remove)
					{
						m_activeSustains.clear();
					}
				}
				else
				{
					if (m_sustainDropLeniencyTimer >= g_const_sustainDropLeniency) // Drop the sustain
					{
						// This will drop the sustain (stop scoring etc)
						for (int i = 0; i < noteAmount; i++)
						{
							m_missedSustains.push_back({ m_notePool[*it].lastScoreCheck, (*it) + i });
						}
						m_activeSustains.clear();

						//m_missedSustains.push_back({ timeOffset, *it });
						//it = m_activeSustains.erase(it);

						m_sustainDropLeniencyTimer = 0;
					}
					else
					{
						m_sustainDropLeniencyTimer += timeDifference;
					}
					
				}
				break;
			}
		}

		m_timerWhammy -= delta;
		if (m_timerWhammy < 0)
			m_timerWhammy = 0;

		// Since there are no sustains active, the game should not be in extended sustain mode, as it may cause problems
		if (m_activeSustains.empty())
		{
			m_extendedSustainMode = false;
			m_effectWhammyData.clear();
		}

		if (m_ignoreRange)
			return;

		auto range = GetRange();

		// Remove any note that moves behind the reaching range
		while (m_noteIndex < m_notePool.size())
		{
			if ((m_notePool[m_noteIndex].time - timeOffset) <= range.x)
			{
				if (m_notePool[m_noteIndex].sustain > m_notePool[m_noteIndex].time)
					m_missedSustains.push_back({ m_notePool[m_noteIndex].time, m_noteIndex });
				ResetStreak(timeOffset, false);
				m_noteIndex++;
				//m_strumHitTimer = INSTRUMENT_TIMER_INACTIVE; // Don't want player to be able to strum, miss, and gain (all on one strum)
			}
			else
				break;
		}

		// Put notes in a range list and remove any that are no longer in range
		// The notes in range go into an array, so that it's easier to iterate through
		//			notes the player can hit.
		int inRangeCounter = m_noteIndex;

		m_notesInRange.clear();

		while (inRangeCounter < m_notePool.size())
		{
			float pos = m_notePool[inRangeCounter].time - timeOffset;
			bool isHopoOrTap = IS_NOTE(m_notePool[inRangeCounter], (NOTE_FLAG_HOPO | NOTE_FLAG_TAP)) > 0;

			if (pos < range.y && pos > range.x)
			{
				m_notesInRange.push_back(inRangeCounter);

				inRangeCounter++;
			}
			else
			{
				break;
			}
		}
	}

	void Instrument::Hit(int noteIndex, int removeAmount, int notesToHit, const double &timeOffset, bool setHitTime, bool addScore, bool increaseNoteStreak, bool resetScore, bool eachNoteIncreasesStreak, bool setHitTimeToOffset)
	{
		Note &baseNote = m_notePool[noteIndex];

		bool lostStarpower = false;
		if (resetScore) lostStarpower = ResetStreak(timeOffset, true);

		// Player hit a note early while in a sustain
		// Give the player the remaining points and remove the note from activeSustains vector
		if (!m_activeSustains.empty() && !m_extendedSustainMode)
		{
			unsigned int note = m_activeSustains[0];
			AddSustainScore(note, m_notePool[noteIndex].chartPos, m_notePool[note].sustain);

			m_activeSustains.clear(); // This is fine as only one note is calculated for the score in non extendedSustainMode. The first note in activeSustain is usually the one to be used in the score calculation
			m_effectWhammyData.clear();
		}

		bool noSustain = m_activeSustains.empty();

		int removedNotes = 0;
		int allRemovedNotes = 0;
		int noteWithLongestSustain = 0;

		for (int i = 0; i < removeAmount; i++)
		{
			Note &currentNote = m_notePool[noteIndex + i];

			if (IS_NOTE(currentNote, NOTE_FLAG_HIT))
				allRemovedNotes++;
			else if (notesToHit & (1 << currentNote.note))
			{
				allRemovedNotes++;
				removedNotes++;

				SET_NOTE(currentNote, NOTE_FLAG_HIT);

				if (setHitTime)
					currentNote.lastScoreCheck = setHitTimeToOffset ? timeOffset : currentNote.time;

				float longSustainTime = m_notePool[noteIndex + noteWithLongestSustain].sustain;
				if (longSustainTime != currentNote.sustain)
				{
					if (longSustainTime < currentNote.sustain)
						noteWithLongestSustain = i;
					m_extendedSustainMode = true;
				}

				if (currentNote.sustain > currentNote.time)
					m_activeSustains.push_back(noteIndex + i); // This activates the sustain
				
				if (currentNote.note == NOTE_OPEN)
					notesToHit = 31;
				
				m_animHit[currentNote.note].Play(ANIM_CONTROL_RESET | ANIM_CONTROL_STOP_ON_END);

				if (eachNoteIncreasesStreak)
				{
					if (increaseNoteStreak)
						m_noteStreak++;
					m_notesHit++;

					int index = GetSectionIndex(m_notePool[noteIndex].chartPos, *m_chart);
					if (index != -1)
						m_sections[index].notesHitInSection++;
				}
			}
		}

		if (!lostStarpower && IS_NOTE(baseNote, NOTE_FLAG_IS_STARPOWER) && allRemovedNotes == removeAmount)
		{
			bool giveStarpower = false;

			if (noteIndex + removeAmount >= m_notePool.size())
				giveStarpower = true;
			else
			{
				Note &next = m_notePool[noteIndex + removeAmount];

				if (IS_NOTE(next, NOTE_FLAG_IS_STARPOWER))
				{
					ChartStarpower currentPhrase = GetStarpowerPhraseFrom(noteIndex);
					ChartStarpower nextPhrase = GetStarpowerPhraseFrom(noteIndex + removeAmount);
					
					if (nextPhrase.timeBegin == -1 || nextPhrase.timeBegin > currentPhrase.timeBegin) // The next note is either not in a phrase, or is in another phrase (-1 = no phrase, here just in case)
					{
						giveStarpower = true;
					}
						
				}
				else
				{
					giveStarpower = true;
				}
					
					
			}
			
			if (giveStarpower)
			{
				m_starpowerAmount += 0.25f;
				m_starpowerPhrasesHit++;
				// Play starpower animations
				for (int i = 0; i < removeAmount; i++)
					m_animStarHit[m_notePool[noteIndex + i].note].Play(ANIM_CONTROL_RESET | ANIM_CONTROL_STOP_ON_END);
			}
		}

		if (!m_extendedSustainMode && noteIndex + removeAmount < m_notePool.size())
			if (m_notePool[noteIndex + removeAmount].time < baseNote.sustain)
				m_extendedSustainMode = true;
		
		m_catcherManager.Hit(notesToHit);

		if (!eachNoteIncreasesStreak)
		{
			if (increaseNoteStreak)
				m_noteStreak++;
			m_notesHit++;

			int index = GetSectionIndex(m_notePool[noteIndex].chartPos, *m_chart);
			if (index != -1)
				m_sections[index].notesHitInSection++;
		}

		if (m_noteStreak > m_highestNoteStreak)
			m_highestNoteStreak = m_noteStreak;

		if (addScore)
			AddNoteScore(removedNotes);

		if (allRemovedNotes == removeAmount)
			m_noteIndex = noteIndex + removeAmount;

		if (noSustain)
		{
			m_effectWhammyData.push_back({ m_lastWhammyValue, 0 });
			m_effectWhammyData.push_back({ 0, 0 });
		}
	}

	bool Instrument::IsNoteInRange(int index, const double &timeOffset)
	{
		if (m_notesInRange.empty())
			return false;
		for (int i = 0; i < m_notesInRange.size(); i++)
		{
			if (m_notesInRange[i] == index)
				return true;
		}
		return false;
	}

	bool Instrument::ResetStreak(const double &timeOffset, bool wasOverstrum)
	{
		if (wasOverstrum && 
			(m_noteIndex == 0 // Prevent overstrumming before the first note of a song or section
				|| m_noteIndex == m_notePool.size())) return false; // Prevent overstrumming after last note

		bool retVal = false;
		m_isFC = false;
		m_playerInCombo = false;

		if (m_noteStreak > 10 && !wasOverstrum)
			m_effectWobble.PlayEffect(EFFECT_TYPE_WOBBLE, EFFECT_FLAG_STOP_ON_END);
			//SetEffect(OFFSET_EFFECT_WOBBLE);

		if (!wasOverstrum) 
			m_notesMissed++;

		m_noteStreak = 0;

		auto starpower = m_starpowerPhrases.begin();
		while (starpower != m_starpowerPhrases.end())
		{
			if (timeOffset < starpower->timeBegin)
				break;
			if (timeOffset >= starpower->timeBegin && timeOffset < starpower->timeEnd)
			{ // Get rid of it
				retVal = true;
				for (int j = m_noteIndex; j < m_notePool.size(); j++)
				{
					if (m_notePool[j].time >= starpower->timeBegin && m_notePool[j].time < starpower->timeEnd)
						CLEAR_NOTE(m_notePool[j], NOTE_FLAG_IS_STARPOWER);
					else
						break;
				}
			}
			
			starpower++;
		}

		return retVal;
	}

	unsigned int Instrument::GetScoreModifier()
	{
		return 
			glm::clamp(
				glm::min(((int)floor(m_noteStreak / 10) + 1), m_maxModifier) * (m_isStarpowerActive ? 2 : 1),
				0,
				12
			);
	}
	unsigned int Instrument::GetMaxScoreModifier()
	{
		return m_maxModifier;
	}

	void Instrument::AddNoteScore(unsigned int noteCount)
	{
		unsigned int score = noteCount * 50;
		m_playerScore += GetScoreModifier() * score;
	}

	bool Instrument::AddSustainScore(unsigned int pointer, const int &pos, const double &timeOffset)
	{
		double time = timeOffset;
		Note &note = m_notePool[pointer];
		int currentChartPos = pos;
		int diff = currentChartPos - note.chartPos;

		if (diff > 0)
		{
			int susScore = StageSystem::GetInstance()->GetChart()->songData.sustainScore;

			while (diff >= susScore && note.chartLen >= susScore)
			{
				m_playerScore += GetScoreModifier();
				note.chartPos += susScore;
				note.chartLen -= susScore;
				diff -= susScore;
			}
			
			if (note.chartLen < susScore)
			{
				m_playerScore += (note.chartLen / susScore) * GetScoreModifier();
				note.chartPos += note.chartLen;
				note.chartLen = 0;
			}
		}
		if (note.lastScoreCheck < timeOffset)
		{
			if (m_extendedSustainMode)
			{
				note.lastScoreCheck = time;
			}
			else
			{
				unsigned int endPointer = GetChordEndingIndex(m_notePool, pointer);
				for (unsigned int i = pointer; i < (pointer + endPointer); i++)
				{
					m_notePool[i].lastScoreCheck = time;
				}
			}
		}
		return note.chartLen <= 0;
	}

	Animator *Instrument::GetAnimsStarpower()
	{
		return m_animStarHit;
	}

	Animator *Instrument::GetAnimsHit()
	{
		return m_animHit;
	}

	int Instrument::GetScore() { return (m_playerScore); }

	int Instrument::GetAmountOfNotesInChart() { return m_maxNotes; }
	int Instrument::GetNotesMissed() { return m_notesMissed; }
	int Instrument::GetNotesHit() { return m_notesHit; }
	int Instrument::GetStreak() { return m_noteStreak; }
	int Instrument::GetHighestStreak() { return m_highestNoteStreak; }
	int Instrument::GetNoteCount() { return m_noteCount; }
	bool Instrument::IsFC() { return m_isFC; } // m_highestNoteStreak is set when the player misses a note

	ScoreDisqualifier Instrument::GetDisqualifications()
	{
		return m_scoreDisqualifier;
	}

	int Instrument::GetInstrumentType()
	{
		return m_parentContainer->m_instrument;
	}
	int Instrument::GetDifficulty()
	{
		return m_parentContainer->m_difficulty;
	}
	bool Instrument::IsPro()
	{
		return m_drumsIsPro;
	}

	unsigned int Instrument::GetNoteCutoff() { return m_noteIndex; }
	const std::vector<unsigned int>	 &Instrument::GetActiveSustains() { return m_activeSustains; }
	const std::vector<SustainMissData> &Instrument::GetMissedSustains() { return m_missedSustains; }
	const std::deque<WhammyEffectData> &Instrument::GetWhammyEffectData() { return m_effectWhammyData; }

	int Instrument::GetBaseScore()
	{
		return m_notePool.size() * 50;
	}

	bool Instrument::IsBotControlling() { return m_isBot; }

	float Instrument::GetStarpowerAmount() { return m_starpowerAmount; }
	bool Instrument::IsStarpowerActive() { return m_isStarpowerActive; }

	ChartStarpower Instrument::GetStarpowerPhraseFrom(unsigned int pointer)
	{
		Note &note = m_notePool[pointer];
		for (auto i = m_starpowerPhrases.begin(); i != m_starpowerPhrases.end(); i++)
		{
			if (note.time < i->timeBegin)
				break;
			if (note.time >= i->timeBegin && note.time < i->timeEnd)
				
				return *i;
		}
		return { 0, 0, 0, -1, -1 };
	}

	void Instrument::ActivateStarpower()
	{
		if (m_starpowerAmount >= 0.5f && !m_isStarpowerActive)
		{
			m_isStarpowerActive = true;
			m_effectBounce.PlayEffect(EFFECT_TYPE_BOUNCE, EFFECT_FLAG_STOP_ON_END);
			//SetEffect(OFFSET_EFFECT_SINK);
		}
	}

	void Instrument::UpdateStarpower(float delta, const double &timeOffset)
	{
		bool activeStar = false;

		if (!m_isBot && (activeStar))// || activeStarAxis))
			ActivateStarpower();

		if (m_isStarpowerActive && timeOffset > StageSystem::GetInstance()->GetTimePaused())//m_stage->GetTimePaused())
		{
			AddToStarpower(delta, timeOffset, true);
		}

		if (m_starpowerAmount <= 0 && m_isStarpowerActive)
			m_isStarpowerActive = false;
	}

	void Instrument::AddToStarpower(float delta, const double &timeOffset, bool negate)
	{
		Chart chart = *StageSystem::GetInstance()->GetChart();
		ChartTS currentTS = GetCurrentTSByTime(timeOffset, chart);

		float measureTime = 1.0f / (GetFullMeasureAt(timeOffset, chart) * currentTS.numerator) / 2;

		if (!negate)
			m_starpowerAmount += (measureTime * delta);
		else
			m_starpowerAmount -= (measureTime * delta);
	}

	const Effect &Instrument::GetEffectBounce() { return m_effectBounce; }
	const Effect &Instrument::GetEffectWobble() { return m_effectWobble; }

	glm::vec4 Instrument::GetRange()
	{
		return ToCorrectRange(
			glm::vec4(m_rangeBack, m_rangeFront, m_rangeBack, m_rangeHOPOFront),
			StageSystem::GetInstance()->GetPlaybackSpeed()
			);
	}

	void Instrument::DoWhammy(float time, float axis)
	{
		bool doWhammy = false;
		if (axis == 0 && m_lastWhammyValue > 0)
		{
			doWhammy = true;
		}
		if (axis == 1 && m_lastWhammyValue < 1)
		{
			doWhammy = true;
		}

		float thresholdTest = axis - m_lastWhammyValue;

		if (thresholdTest < -0.1f || thresholdTest > 0.1f || doWhammy)
		{
			// Starpower
			m_timerWhammy = 0.3f;

			m_lastWhammyValue = axis;
			// Whammy Animation
			if (!m_activeSustains.empty())
			{
				//LOG_INFO("Whammy Axis {0}", whammyAxis);
				if (m_effectWhammyData.empty())
					m_effectWhammyData.push_back({ m_lastWhammyValue, 0 });
				else
					m_effectWhammyData.push_front({ m_lastWhammyValue, 0 });
				
			}

			/*if (m_effectWhammyData.empty())
			{
				LOG_INFO("Whammy {0}", m_lastWhammyValue);
			}
			else
			{
				LOG_INFO("Whammy {0}: Wibble {1}", m_lastWhammyValue, m_effectWhammyData.front().stretch);
			}*/
		}
		
	}

	void Instrument::_OnInput(void *data)
	{
		if (!m_created)
			return;
		InputEventData *iData = (InputEventData*) data;
		if ((!m_isBot && StageSystem::GetInstance()->IsPlaying() && m_parentContainer->m_inputHandle == iData->device->handle))
			PlayerInput(iData);
	}
}

