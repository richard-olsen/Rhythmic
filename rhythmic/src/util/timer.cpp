#include "timer.hpp"

#include <wge/core/time.hpp>

namespace Rhythmic
{
  static std::vector<Timer*> g_timers;


  Timer::Timer(double time, const std::function<void()> &callback) :
    time(time),
    callback(callback)
  { }

  Timer::~Timer()
  {
    Stop();
  }

  void Timer::StartTimer()
  {
    auto it = std::find(g_timers.begin(), g_timers.end(), this);
    if (it == g_timers.end())
      g_timers.push_back(this);
  }
  void Timer::Stop()
  {
    auto it = std::find(g_timers.begin(), g_timers.end(), this);
    if (it != g_timers.end())
      g_timers.erase(it);
  }

  void TickAllTimers()
  {
    if (g_timers.empty())
      return;

    auto it = g_timers.begin();

		while (it != g_timers.end())
		{
			(*it)->time -= WGE::Core::TimeStep.m_timeStep;

			if ((*it)->time <= 0) // Timer triggers
			{
				(*it)->callback();
				it = g_timers.erase(it);
			}
			else // Timer hasn't elapsed the full amount of time
			{
				it++;
			}
		}
  }

  void CleanTimers()
  {
    g_timers.clear();
  }
}