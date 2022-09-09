#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <functional>

namespace Rhythmic
{
  /*
  Timers allow for functions to be called after a certain
  time has passed. Timers tick on the main thread! If the
  main thread is blocked by anything, Timers will NOT work
  */
  struct Timer
  {
    double time;
    const std::function<void()> callback;

    Timer(double time, const std::function<void()> &callback);
    ~Timer();

    void StartTimer();
    void Stop();
  };

  /*
  Called By Engine!
  Ticks every timer
  */
  void TickAllTimers();

  /*
  Called By Engine!
  Clears ALL timers. The timers will NOT call their callback functions
  */
  void CleanTimers();
}

#endif

