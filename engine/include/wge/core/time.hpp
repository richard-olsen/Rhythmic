#ifndef WGE_TIME_HPP_
#define WGE_TIME_HPP_

namespace WGE
{
	namespace Core
	{
		double GetTimeInSeconds();

		struct _TimeStep
		{
			float m_fixedTimeStep;
			float m_timeStep;
		};
		extern _TimeStep TimeStep;

	}
}

#endif

