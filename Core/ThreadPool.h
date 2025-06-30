#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

/**
 * @brief Simple thread pool for running tasks asynchronously.
 *
 * Spawns a fixed number of worker threads that execute tasks submitted to the queue.
 * Useful for dispatching updates, async jobs, and lightweight parallelism.
 */
class ThreadPool
{
public:
	 /**
	 * @brief Construct a new Thread Pool.
	 *
	 * @param threadCount Number of worker threads to spawn.
	 *                    Defaults to hardware concurrency.
	 */
	explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());

	/**
	 * @brief Destroy the Thread Pool, waits for all threads to finish.
	 */
	~ThreadPool();

	 /**
	 * @brief Submit a task to the pool.
	 *
	 * @tparam Func Callable type.
	 * @tparam Args Argument types.
	 * @param f Callable to execute.
	 * @param args Arguments to pass.
	 * @return std::future for the result of the task.
	 */
	template<typename Func, typename... Args>
	auto Submit(Func&& f, Args&&... args)
		-> std::future<typename std::invoke_result_t<Func, Args...>>
	{
		using ReturnType = typename std::invoke_result_t<Func, Args...>;

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
		);

		std::future<ReturnType> future = task->get_future();

		{
			std::unique_lock lock(m_mutex);
			m_tasks.emplace([task]() { (*task)(); });
		}

		m_cv.notify_one();
		return future;
	}

private:
	std::vector<std::thread> m_threads;
	std::queue<std::function<void()>> m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_done;

	/**
	 * @brief Start the worker threads.
	 *
	 * This internal helper spawns `threadCount` worker threads.
	 * Each thread loops, waiting for tasks to be submitted to the task queue.
	 *
	 * This method is called automatically by the constructor.
	 * Users should not call it manually.
	 *
	 * @param threadCount Number of worker threads to create.
	 */
	void Start(size_t threadCount);

	/**
	 * @brief Stop the thread pool and join all worker threads.
	 *
	 * Signals all threads to finish processing tasks and exit.
	 * Wakes any threads waiting for tasks.
	 * Waits for all worker threads to join.
	 *
	 * This is called automatically by the destructor,
	 * but can also be called manually if needed.
	 */
	void Stop();
};
