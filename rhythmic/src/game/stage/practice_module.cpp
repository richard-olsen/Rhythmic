#include "practice_module.hpp"

#include "../stage/stage.hpp"

#include "../../chart/chart.hpp"

namespace Rhythmic
{
	PracticeModule::PracticeModule(StageSystem *parent) :
		m_parent(parent),
		m_startPos(0),
		m_startTime(0),
		m_endPos(0),
		m_endTime(0),
		m_sectionSelection(0)
	{
	}

	PracticeModule::~PracticeModule()
	{
	}

	void PracticeModule::SetTime(int start, int end)
	{
		m_startPos = start;
		m_endPos = end;

		Chart *chart = StageSystem::GetInstance()->GetChart();

		m_startTime = GetAdjustedTimeFromPosition(m_startPos, *chart);
		m_endTime = GetAdjustedTimeFromPosition(m_endPos, *chart);

		ResetAB();
	}
	void PracticeModule::SetTimeToWholeSong()
	{
		m_startTime = 0;
		m_endTime = m_parent->GetMusicPlayer()->GetSyncLength();
		m_startPos = 0;
		m_endPos = m_parent->GetChart()->lastNotePosWithSustain;

		ResetAB();
	}

	void PracticeModule::SetTimeAOffset(int pos)
	{
		m_posAOffset = pos;

		Chart* chart = StageSystem::GetInstance()->GetChart();

		m_timeAOffset = GetAdjustedTimeFromPosition(m_posAOffset, *chart);
	}

	void PracticeModule::SetTimeBOffset(int pos)
	{
		m_posBOffset = pos;

		Chart* chart = StageSystem::GetInstance()->GetChart();

		m_timeBOffset = GetAdjustedTimeFromPosition(m_posBOffset, *chart);
	}

	void PracticeModule::ResetAB()
	{
		m_posAOffset = m_startPos;
		m_posBOffset = m_endPos;
		m_timeAOffset = m_startTime;
		m_timeBOffset = m_endTime;
	}

	float PracticeModule::GetTimeAOffset()
	{
		return m_timeAOffset;
	}

	float PracticeModule::GetTimeBOffset()
	{
		return m_timeBOffset;
	}

	int PracticeModule::GetPositionA()
	{
		return m_posAOffset;
	}

	int PracticeModule::GetPositionB()
	{
		return m_posBOffset;
	}

	void PracticeModule::SetPracticeMode(bool isPracticeMode)
	{
		m_isPracticeMode = isPracticeMode;
		if (isPracticeMode)
			m_sectionSelection = 0;
	}
	bool PracticeModule::IsPracticeMode() { return m_isPracticeMode; }

	void PracticeModule::SetTargetButton(int button)
	{
		m_sectionSelection = button;
	}
	int PracticeModule::GetTargetButton()
	{
		return m_sectionSelection;
	}

	void PracticeModule::PrepareStage()
	{
		//m_parent->SetPause(false);
		//m_parent->SetPause(true);
		m_parent->SetDuration(GetTimeEndOffsetted() + (1.0f * m_parent->GetPlaybackSpeed()));
		m_parent->SetSongTime(GetTimeStartOffsetted() - (1.5f * m_parent->GetPlaybackSpeed()));
	}

	// Offsetted times

	int PracticeModule::GetPositionStartOffsetted()
	{
		return m_posAOffset;
	}

	int PracticeModule::GetPositionEndOffsetted()
	{
		return m_posBOffset;
	}

	float PracticeModule::GetTimeStartOffsetted()
	{
		return m_timeAOffset;
	}
	float PracticeModule::GetTimeEndOffsetted()
	{
		return m_timeBOffset;
	}

	//

	float PracticeModule::GetTimeStart()
	{
		return m_startTime;
	}
	float PracticeModule::GetTimeEnd()
	{
		return m_endTime;
	}

	int PracticeModule::GetPositionStart()
	{
		return m_startPos;
	}
	int PracticeModule::GetPositionEnd()
	{
		return m_endPos;
	}
}

