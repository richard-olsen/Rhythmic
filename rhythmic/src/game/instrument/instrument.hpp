#ifndef INSTRUMENT_H_
#define INSTRUMENT_H_

#include "../catchers/catcher_manager.hpp"
#include "../notes/note.hpp"
#include "../notes/beatline.hpp"
#include "../notes/note_util.hpp"
#include "../notes/note_renderer.hpp"
#include "../../chart/chart.hpp"
#include "../../input/input.hpp"
#include "../../rendering/animation.hpp"
#include "../../rendering/effect.hpp"
#include "../../events/event_system.hpp"
#include <vector>
#include <limits>

namespace Rhythmic
{
	constexpr float g_const_starpowerAmountRequired = 0.5f; // The defined amount of starpower required in order for it to be activated

	constexpr float INSTRUMENT_TIMER_INACTIVE = -3.0f;

	inline bool TimerExpired(float &timer, bool reset = true)
	{
		bool expired = (timer > INSTRUMENT_TIMER_INACTIVE) && (timer <= 0);

		if (expired && reset)
			timer = INSTRUMENT_TIMER_INACTIVE;

		return expired;
	}

	constexpr int g_const_offsetEffectNone = 0; // No effect
	constexpr int g_const_offsetEffectWobble = 1; // Wobbles the fretboard
	constexpr int g_const_offsetEffectSink = 2; // Causes fretboard to sink, and come right back up

	// (Increase if still dropping sustains. Decrease if you can let go then press down again and it still holds) 
	// (Both shouldn't be possible because humans and fret travel time)
	constexpr float g_const_sustainDropLeniency = 0.0125f; // 12.5 milliseconds

	class Stage;
	struct ContainerPlayer;

	// Values that disqualify a players score
	// Disqualifying a player's score means that it is not allowed to be on leader boards (if those are ever implemented)
	//		for now, they just display in yellow text that a specific modifier was used

	struct ScoreDrumsDisqualifier
	{
		bool autoKick; // Debating whether this should be a thing or not
	};

	struct Score5FretDisqualifier
	{
		bool noStrum;
	};
	
	struct ScoreDisqualifier
	{
		ScoreDrumsDisqualifier drums;
		union
		{
			Score5FretDisqualifier guitar;
			Score5FretDisqualifier bass;
		};
	};

	constexpr int INSTRUMENT_CAN_HIT_NOTE_1 = (1 << NOTE_1); // 0000 0001 = 1
	constexpr int INSTRUMENT_CAN_HIT_NOTE_2 = (1 << NOTE_2); // 0000 0010 = 2
	constexpr int INSTRUMENT_CAN_HIT_NOTE_3 = (1 << NOTE_3); // 0000 0100 = 4
	constexpr int INSTRUMENT_CAN_HIT_NOTE_4 = (1 << NOTE_4); // 0000 1000 = 8
	constexpr int INSTRUMENT_CAN_HIT_NOTE_5 = (1 << NOTE_5); // 0001 0000 = 16
	constexpr int INSTRUMENT_CAN_HIT_NOTE_OPEN = (1 << NOTE_OPEN); // 0010 0000 = 32
	constexpr int INSTRUMENT_CAN_HIT_NOTE_CYMBAL_1 = (1 << (NOTE_2 + 5)); // 0100 0000 = 64
	constexpr int INSTRUMENT_CAN_HIT_NOTE_CYMBAL_2 = (1 << (NOTE_3 + 5)); // 1000 0000 = 128
	constexpr int INSTRUMENT_CAN_HIT_NOTE_CYMBAL_3 = (1 << (NOTE_4 + 5)); // 0001 0000 0000 = 256

	constexpr int INSTRUMENT_MISS_FLAG_OUT_OF_RANGE = (1 << 0);
	constexpr int INSTRUMENT_MISS_CATCHER_ACTIVE_ON_NO_NOTE = (1 << 1);

	struct InstrumentCanHitDetail
	{
		bool canCatchersHit;
		int missFlag;
		int notesToHit;
		int allNotes;
		int noteCount;
	};

	class Instrument
	{
	public:
		Instrument(bool ignoreRange);
		~Instrument();

		/*
		Initializes the Instrument
		*/
		void Create(ContainerPlayer *container, float videoCalibration = 0);
		/*
		Destroys the instrument
		*/
		void Destroy();

		/*
		Resets the instruments values and replaces all the notes to
		allow players to restart
		*/
		void Reset();

		/*
		A function to update the instrument
		*/
		void Update(const float &delta, const double &timeOffset);

		/*
		A function for the drums that corrects catchers
		*/
		virtual void FixActiveCatchers(const float &delta, const double &timeOffset, int active) = 0;
		
		/*
		A function for gameplay features (testing, hitting, missing notes)
		*/
		void UpdateGameplay(const float &delta, const double &timeOffset);

		/*
		Updates the instruments timers
		*/
		virtual void UpdateTimers() = 0;

		/*
		Updates player's instrument (checks for hit notes etc)
		*/
		virtual void UpdatePlayer(const double &timeOffset) = 0;
		/*
		Player input for instruments, Instrument type dependent
		*/
		virtual void PlayerInput(InputEventData *data) = 0;

		/*
		Updates bot instrument
		*/
		virtual void UpdateBot(const double &timeOffset) = 0;

		/*
		Function is virtual as it is instrument dependent

		A function that determins the notes and calls hit.

		Will determin if the note needs a strumming, controls m_playerInCombo

		returns true if the note has been hit
		*/
		virtual bool HitHelper(int pointer, int removeAmount, bool strummed, const double &timeOffset, bool forceStreakReset = false, bool isStrumAhead = false) = 0;

		/*
		A function to tell the player to hit a current note
		*/
		void Hit(int pointer, int removeAmount, int notesToHit, const double &timeOffset, bool setHitTime = true, bool addScore = true, bool increaseNoteStreak = true, bool resetScore = false, bool eachNoteIncreasesStreak = false, bool setHitTimeToOffset = false);

		/*
		Function is virtual as it is instrument dependent

		Determins if the catchers can indeed hit a specific note
		*/
		virtual InstrumentCanHitDetail CanCatchersHitNote(int pointer, int removeAmount, const double &timeOffset, bool ignoreTime = false, bool keepExtendedSustains = false) = 0;

		/*
		Function called after stage continues. The function
		queries the player's controller to see which buttons
		are held or released, and corrects all the values so
		nothing is held down by a ghost
		*/
		virtual void CorrectInputValues() = 0;

		/*
		Returns true if the note is within range of the catchers
		*/
		bool IsNoteInRange(int pointer, const double &timeOffset);

		/*
		Resets note streak, and score multiplier, as well as setting the highest note streak (if it is the highest)
		Returns true if the player lost their starpower phrase
		*/
		bool ResetStreak(const double &timeOffset, bool wasOverstrum);

		/*
		Returns score modifier
		*/
		unsigned int GetScoreModifier();
		unsigned int GetMaxScoreModifier();

		/*
		Adds score for the notes
		*/
		void AddNoteScore(unsigned int noteCount);
		/*
		Adds score for sustain notes
		returns true when the sustain is finished
		*/
		bool AddSustainScore(unsigned int pointer, const int &pos, const double &timeOffset);

		/*
		Returns true if the bot is active on this instrument
		*/
		bool IsBotControlling();

		// Stuff for starpower

		/*
		Returns the amount of starpower the instrument has (0 - 1, it's a percentage)
		*/
		float GetStarpowerAmount();

		/*
		Returns if the starpower is active
		*/
		bool IsStarpowerActive();

		/*
		Activates starpower
		*/
		void ActivateStarpower();

		/*
		Returns the current score held by the instrument
		*/
		int GetScore();

		/*
		Returns all starpower anims (the size is 6)
		*/
		Animator *GetAnimsStarpower();

		/*
		Returns all hit anims (the size is 6)
		*/
		Animator *GetAnimsHit();

		int GetAmountOfNotesInChart();
		int GetNotesMissed();
		int GetNotesHit();
		int GetStreak();
		int GetHighestStreak();
		int GetNoteCount();
		bool IsFC();

		ScoreDisqualifier GetDisqualifications();
		int GetInstrumentType();
		int GetDifficulty();
		bool IsPro();

		/*
		Returns the cutoff for the notes to be rendered. (increases as the game continues as it's intended to hide missed and hit notes)
		*/
		unsigned int GetNoteCutoff();
		/*
		Returns the NotePool of playable notes
		*/
		NotePool *GetNotes();
		/*
		Returns the NotePool of notes converted from the chart
		*/
		NotePool *GetNotePoolTemplate();
		/*
		Returns the SectionPool of sections converted from the chart
		*/
		std::vector<Section> *GetSections();

		CatcherManager *GetCatcherManager();

		/*
		Returns a list of currently active sustain notes
		*/
		const std::vector<unsigned int>	 &GetActiveSustains();
		/*
		Returns a list of missed sustain note data. Intended for missed sustain visual effects
		*/
		const std::vector<SustainMissData> &GetMissedSustains();
		/*
		Returns a list of whammy wibble data, for the whammy wibble effect
		*/
		const std::deque<WhammyEffectData> &GetWhammyEffectData();

		/*
		Returns the base score of the current chart loaded into the instrument
		*/
		int GetBaseScore();

		//const glm::vec2 &GetOffset();
		//void SetEffect(int effect);
		const Effect &GetEffectBounce();
		const Effect &GetEffectWobble();

		/*
		Returns the range used by the instrument
		 x - back range
		 y - front range
		 z - back hopo range
		 w - front hopo range
		 Check Later: Is a z component needed?
		*/
		glm::vec4 GetRange();

		/*
		Does whammy animation and rewards player with extra starpower from sustain starpower notes
		*/
		void DoWhammy(float time, float axis);

		/*
		Listens for the ET_INPUT event, and sends the data to instruments via PlayerInput function
		Will NOT call PlayerInput if isBot is true
		*/
		void _OnInput(void *);

		/*
		Makes the given range adjusted for song speed and video calibration
		*/
		template<typename T>
		inline T ToCorrectRange(const T &range, float speed)
		{
			return (range * glm::max(speed, 1.0f)) - m_videoCalibration;
		}

	protected:
		EventListener					m_inputListener;						// Input listener

		ContainerPlayer					*m_parentContainer;						// Access to player's container
		bool							m_created;								// Is true when Instrument is properly created

		ScoreDisqualifier				m_scoreDisqualifier;					// Score disqualifiers

		float							m_videoCalibration;						// Video calibration

		float							m_lastWhammyValue;						// The last recorded whammy value
		float							m_currentWhammyValue;					// The current whammy value
		float							m_timerWhammy;							// Whammy timer

		bool							m_isBot;								// Determines if the instrument is controlled by the ai
		bool							m_isReplay;								// Determines if the instrument is fed artificial inputs

		Chart							*m_chart;								// Instance of the chart, incase the instrument needs access to it
		NotePool						m_holdingPool;							// Holding pool keeps a copy of translated notes
		NotePool						m_notePool;								// A pool of notes for the instrument
		std::vector<Section>			m_sections;								// A list of sections for the instrument to keep track of stats
		CatcherManager					m_catcherManager;						// Catcher manager for the instrument
		std::vector<unsigned int>		m_activeSustains;						// Contains a list of pointers to active sustains
		std::vector<SustainMissData>	m_missedSustains;						// Contains a list of sustain data for missed sustains (for visual purposes)
		
		std::deque<WhammyEffectData>	m_effectWhammyData;						// The list of whammy effect data, containing stretch value and time

		//Pool<unsigned int>				m_notesInRange;							// Contains a list of pointers to in range notes
		std::vector<unsigned int>		m_notesInRange;

		// Starpower variables
		float							m_starpowerAmount;
		bool							m_isStarpowerActive;
		std::vector<ChartStarpower>		m_starpowerPhrases;						// Contains the chart's phrases

		ChartStarpower GetStarpowerPhraseFrom(unsigned int note);
		void UpdateStarpower(float delta, const double &timeOffset);
		void AddToStarpower(float delta, const double &timeOffset, bool negate = false);

		unsigned int					m_noteCount;							// Total notes in section(s)
		unsigned int					m_noteIndex;							// Is the current note in the note pool to test/hit
		unsigned int					m_noteStreak;							// Value for the note streak counter
		unsigned int					m_notesHit;								// The amount of notes hit
		unsigned int					m_notesMissed;
		unsigned int					m_maxNotes;								// The amount of notes the chart contains
		unsigned int					m_highestNoteStreak;					// Value of the highest note streak (to be reported back to the player after the song)
		bool							m_playerInCombo;						// Keeps track if the player is in combo, useful for allowing/disallowing the player to hit HOPOs without strumming
		bool							m_extendedSustainMode;					// Active when the player is currently holding an extended sustain
		bool							m_playerChangedCatchersSinceLastHit;	// Must be true to hit the taps. Player can not hold down a fret and continuously get taps in a row
		int								m_lastActiveCatchers;					// Value of the catchers that where last active when hitting taps and hopos
		int								m_playerScore;							// Player's score
		int								m_baseScore;							// The base score; used for average multiplier calculation
		int								m_ghostCount;
		unsigned int					m_starpowerPhrasesHit;					// Keeps count of how many starpower phrases the player got

		int								m_maxModifier;							// It's usually 4, but bass can go up to 6

		bool							m_isFC;									// Is Full Combo?

		float							m_lastTapHitTime;						// The value of when the last tap/hopo note is hit
		float							m_lastStrumHitTime;						// The value of when the last strum note is hit

		float							m_sustainDropLeniencyTimer;

		float							m_rangeBack;							// The back range for any note to be considered in range
		float							m_rangeFront;							// The front range for any note to be considered in range

		float							m_rangeHOPOFront;						// This is the front range of the HOPO

		bool							m_ignoreRange;							// Tells the instrument to or not to ignore the range (if true, notes will not be removed when they pass a certain point)

		Animator						m_animStarHit[6];						// Enough for 5 pads and the open/kick note (just in case, I might not even have one for it?)
		Animator						m_animHit[6];							// Enough for 5 pads and the open/kick note. The animation when the note is hit (in GH it's a flame, in RB it's some broken glass)

		Effect							m_effectBounce;
		Effect							m_effectWobble;

		bool							m_drumsIsPro;
		bool							m_drumsIs4Lane;
	};
}

#endif

