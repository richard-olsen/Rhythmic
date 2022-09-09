#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <functional>

namespace Rhythmic
{
	namespace ThreadPool
	{
		void Initialize();
		void Destroy();

		void QueueJob(std::function<void()> job);
	}
	inline void async(std::function<void()> job)
	{
		ThreadPool::QueueJob(job);
	}
}

#endif

