#include "thread_pool.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

namespace Rhythmic
{
	namespace ThreadPool
	{
		std::mutex g_tpQueueLock;
		std::condition_variable g_tpCondition;
		std::vector<std::thread> g_tpThreads;
		std::queue<std::function<void()>> g_tpWork;
		bool g_tpStop;

		void ThreadFunc()
		{
			while (true)
			{
				std::function<void()> work;

				{
					std::unique_lock<std::mutex> lock(g_tpQueueLock);
					g_tpCondition.wait(lock, []() { return g_tpStop || !g_tpWork.empty(); });

					if (g_tpStop || g_tpWork.empty())
						return;

					work = std::move(g_tpWork.front());
					g_tpWork.pop();
				}

				work();
			}
		}

		void Initialize()
		{
			g_tpStop = false;

			size_t threadCount = std::thread::hardware_concurrency();

			for (int i = 0; i < threadCount; i++)
				g_tpThreads.emplace_back(std::thread(ThreadFunc));
		}

		void Destroy()
		{
			g_tpStop = true;
			g_tpCondition.notify_all();
			for (int i = 0; i < g_tpThreads.size(); i++)
				g_tpThreads[i].join();
		}

		void QueueJob(std::function<void()> job)
		{
			{
				std::unique_lock<std::mutex> lock(g_tpQueueLock);
				g_tpWork.push(job);
			}
			g_tpCondition.notify_one();
		}
	}
}