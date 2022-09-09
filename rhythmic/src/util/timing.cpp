#include "timing.hpp"

namespace Rhythmic
{
	Timing *Timing::GetInstance()
	{
		static Timing timing;
		return &timing;
	}

	double Timing::GetTickRate()
	{
		return m_tickRate;
	}
	double Timing::GetRenderRate()
	{
		return m_renderRate;
	}
	void Timing::SetTickRate(double ticksPerSecond)
	{
		m_tickRate = 1.0 / ticksPerSecond;
	}
	void Timing::SetRenderRate(double framesPerSecond)
	{
		m_renderRate = 1.0 / framesPerSecond;
	}

	Timing::Timing() :
		m_tickRate(1.0 / 1000.0),
		m_renderRate(1.0 / 60.0)
	{ }
}