#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount)
	: m_done(false)
{
	Start(threadCount);
}

ThreadPool::~ThreadPool()
{
	Stop();
}

void ThreadPool::Start(size_t threadCount)
{
	for(size_t i = 0; i < threadCount; ++i)
	{
		m_threads.emplace_back([this]()
							   {
								   while(!m_done.load())
								   {
									   std::function<void()> task;

									   {
										   std::unique_lock lock(m_mutex);
										   m_cv.wait(lock, [this]() { return m_done || !m_tasks.empty(); });

										   if(m_done && m_tasks.empty())
											   return;

										   task = std::move(m_tasks.front());
										   m_tasks.pop();
									   }

									   task();
								   }
							   });
	}
}

void ThreadPool::Stop()
{
	m_done.store(true);
	m_cv.notify_all();

	for(auto& t : m_threads)
		if(t.joinable())
			t.join();
}
