#ifndef RHYTHMIC_PRACTICE_MANAGER_HPP_
#define RHYTHMIC_PRACTICE_MANAGER_HPP_

#include "../../util/singleton.hpp"
#include "../../events/event_system.hpp"

#include "../../chart/chart.hpp"

namespace Rhythmic
{
	class StageSystem;
	class PracticeModule
	{
		friend class StageSystem;
	public:
		PracticeModule(StageSystem *parent);
		~PracticeModule();

		//void SetTime(float start, float end); // This function sets the start and end time of what the player wants to practice
		void SetTime(int start, int end); // This function sets the start and end time of what the player wants to practice
		void SetTimeToWholeSong();

		void SetTimeAOffset(int pos);
		void SetTimeBOffset(int pos);

		float GetTimeAOffset();
		float GetTimeBOffset();
		int GetPositionA();
		int GetPositionB();

		void ResetAB();

		void SetPracticeMode(bool isPracticeMode);
		bool IsPracticeMode();

		void SetTargetButton(int button);
		int GetTargetButton();

		void PrepareStage();

		int GetPositionStartOffsetted();
		int GetPositionEndOffsetted();

		float GetTimeStartOffsetted();
		float GetTimeEndOffsetted();

		float GetTimeStart();
		float GetTimeEnd();
		int GetPositionStart();
		int GetPositionEnd();
	private:
		StageSystem *m_parent;
		float m_startTime;
		float m_endTime;
		int m_startPos;
		int m_endPos;

		int m_sectionSelection;

		float m_timeAOffset = 0.0f;
		float m_timeBOffset = 0.0f;
		int m_posAOffset = 0;
		int m_posBOffset = 0;

		float m_songDuration;

		bool m_isPracticeMode;
	};
}

#endif

