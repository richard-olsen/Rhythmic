#include "animation.hpp"

#include "../util/misc.hpp"

namespace Rhythmic
{
	Animator::Animator() :	
		m_sheetReference(),
		m_frames(0),
		m_frameIndex(0),
		m_stopOnFinished(false),
		m_paused(false),
		m_lastTime(Util::GetTime()),
		m_fps(1.0 / 60.0)
	{ }
	Animator::Animator(const std::vector<AnimFrame> &frames) :
		m_sheetReference(),
		m_frames(frames),
		m_frameIndex(0),
		m_stopOnFinished(false),
		m_paused(false),
		m_lastTime(Util::GetTime()),
		m_fps(1.0/60.0)
	{ }
	Animator::~Animator()
	{ }

	void Animator::SetSheet(const SheetReference &sheet)
	{
		m_sheetReference = sheet;
	}

	void Animator::SetFrames(const std::vector<AnimFrame> &frames)
	{
		m_frames = frames;
	}

	void Animator::SetFPS(float targetFrames)
	{
		m_fps = 1.0 / ((double)targetFrames);
	}

	void Animator::SetFrameIndex(unsigned int index)
	{
		m_frameIndex = index;
		if (m_frameIndex > m_frames.size() && m_frames.size() != 0)
			m_frameIndex = m_frames.size() - 1;
	}

	void Animator::Animate()
	{
		if (!m_paused)
		{
			double time = Util::GetTime();

			if (time - m_lastTime >= m_fps)
			{
				m_frameIndex++;
				m_lastTime = time;
			}

			if (m_frameIndex >= m_frames.size())
			{
				if (m_stopOnFinished)
					m_frameIndex--;
				else
					m_frameIndex = 0;
			}
		}
	}

	void Animator::Play(unsigned int controls)
	{
		if (controls & ANIM_CONTROL_RESET)
			m_frameIndex = 0;
		
		if (controls & ANIM_CONTROL_STOP_ON_END)
			m_stopOnFinished = true;
		else
			m_stopOnFinished = false;

		if (controls & ANIM_CONTROL_PAUSE)
		{
			m_paused = true;
		}
		else
		{
			m_paused = false;
			m_lastTime = Util::GetTime();
		}
	}

	void Animator::Draw(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, bool mirror)
	{
		assert(m_frameIndex < m_frames.size());
		AnimFrame &frame = m_frames[m_frameIndex];
		m_sheetReference.data->AddSprite(frame.tileX, frame.tileY, frame.tileExtensionX, frame.tileExtensionY, pos, size, color, mirror);
	}
}