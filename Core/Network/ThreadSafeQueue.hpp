#pragma once

/**
 * @file ThreadSafeQueue.hpp
 * @brief A simple multi-producer, multi-consumer thread-safe queue.
 */

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @class ThreadSafeQueue
 * @brief Provides safe push/pop between threads.
 * @tparam T The type stored in the queue.
 */
template <typename T>
class ThreadSafeQueue
{
public:
	/**
	 * @brief Push an item into the queue.
	 * @param item The item to add.
	 */
	void push(const T& item)
	{
		{
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(item);
		}
		cond_.notify_one();
	}

	/**
	 * @brief Pop an item from the queue.
	 * @param item The popped item will be stored here.
	 * @return True if an item was popped, false if empty.
	 */
	bool pop(T& item)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if(queue_.empty()) return false;
		item = queue_.front();
		queue_.pop();
		return true;
	}

	/**
	 * @brief Block until an item is available, then pop it.
	 * @param item The popped item.
	 */
	void waitPop(T& item)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] { return !queue_.empty(); });
		item = queue_.front();
		queue_.pop();
	}

	/**
	 * @brief Get current size.
	 * @return Number of items.
	 */
	size_t size() const
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.size();
	}

private:
	mutable std::mutex mutex_;             /**< Mutex for thread safety. */
	std::condition_variable cond_;         /**< Notifies waiting threads. */
	std::queue<T> queue_;                  /**< Internal storage. */
};
