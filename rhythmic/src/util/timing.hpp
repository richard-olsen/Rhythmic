#ifndef TIMING_HPP_
#define TIMING_HPP_

namespace Rhythmic
{
	class Timing
	{
	public:
		static Timing *GetInstance();

		double GetTickRate();
		double GetRenderRate();
		void SetTickRate(double ticksPerSecond);
		void SetRenderRate(double framesPerSecond);
	private:
		Timing();

		double m_tickRate;
		double m_renderRate;
	};
}

#endif

