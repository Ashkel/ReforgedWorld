#pragma once

#include <Core/ThreadPool.hpp>
#include <Core/Utility/Time.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <string>
#include <future>
#include <vector>

/**
 * @brief Async logger with safe shutdown.
 *
 * Logs to file & console using ThreadPool.
 * Guarantees all logs complete before destruction.
 */
class AsyncLogger
{
public:
    /**
     * @brief Create an async logger.
     *
     * @param filename Log file path.
     * @param threads Number of worker threads.
     */
    AsyncLogger(const std::string& filename, size_t threads = 1)
        : m_pool(threads), m_file(filename, std::ios::out | std::ios::app)
    {
        if(!m_file)
            throw std::runtime_error("Failed to open log file: " + filename);
    }

    /**
     * @brief Destructor: waits for all pending log tasks.
     */
    ~AsyncLogger()
    {
        WaitAll();
        m_file.flush();
        m_file.close();
    }

    /**
     * @brief Log a message asynchronously.
     *
     * @tparam Args Any args to format.
     * @param args Message parts.
     * @return std::future<void> for the log task.
     */
    template<typename... Args>
    std::shared_future<void> Log(Args&&... args)
    {
        auto timestamp = Time::CurrentTimestamp();

        std::ostringstream oss;
        oss << "[" << timestamp << "]";
        ((oss << " " << args), ...);

        auto line = oss.str();

        auto fut = m_pool.Submit([this, line]()
                                 {
                                     {
                                         std::scoped_lock lock(m_mutex);
                                         m_file << line << std::endl;
                                         m_file.flush();
                                     }
                                     {
                                         std::scoped_lock lock(m_mutex);
                                         std::cout << line << std::endl;
                                     }
                                 });

        {
            std::scoped_lock lock(m_futuresMutex);
            m_futures.emplace_back(std::move(fut));
        }

        // Return last added future
        return m_futures.back().share();
    }

    /**
     * @brief Wait for all queued log tasks.
     */
    void WaitAll()
    {
        std::scoped_lock lock(m_futuresMutex);
        for(auto& fut : m_futures)
        {
            if(fut.valid())
                fut.get();
        }
        m_futures.clear();
    }

private:
    ThreadPool m_pool;
    std::ofstream m_file;
    std::mutex m_mutex;

    // Store futures for shutdown
    std::vector<std::future<void>> m_futures;
    std::mutex m_futuresMutex;

};