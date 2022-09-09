#include <wge/core/time.hpp>

#include <chrono>

namespace WGE
{
	namespace Core
	{
		double GetTimeInSeconds()
		{
			static auto epoch = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - epoch).count() / 1000000000.0;
		}

		_TimeStep TimeStep;
	}
}

