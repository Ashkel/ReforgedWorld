#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>
#include <future>


/**
 * @brief Connection handle for scoped automatic unsubscribe.
 */
class EventConnection
{
public:
    using UnsubFn = std::function<void()>;

    EventConnection() = default;
    explicit EventConnection(UnsubFn fn) : m_Unsub(std::move(fn)) {}

    ~EventConnection()
    {
        if(m_Unsub)
            m_Unsub();
    }

    EventConnection(const EventConnection&) = delete;
    EventConnection& operator=(const EventConnection&) = delete;

    EventConnection(EventConnection&& other) noexcept
        : m_Unsub(std::move(other.m_Unsub))
    {
        other.m_Unsub = nullptr;
    }

    EventConnection& operator=(EventConnection&& other) noexcept
    {
        if(this != &other)
        {
            if(m_Unsub)
                m_Unsub();
            m_Unsub = std::move(other.m_Unsub);
            other.m_Unsub = nullptr;
        }
        return *this;
    }

    void Disconnect()
    {
        if(m_Unsub)
        {
            m_Unsub();
            m_Unsub = nullptr;
        }
    }

private:
    UnsubFn m_Unsub = nullptr;
};

/**
 * @brief Generic event system with safe subscribe/unsubscribe.
 *
 * @tparam EventEnum Enum type identifying each event.
 */
template<typename EventEnum>
class EventProvider
{
public:
    EventProvider() = default;

    /// Subscribe a simple event.
    EventConnection Subscribe(EventEnum event, std::function<void()> cb)
    {
        std::unique_lock lock(m_Mutex);
        HandlerID id = ++m_NextID;
        m_SimpleEvents[event][id] = std::move(cb);

        return EventConnection([=]()
                               {
                                   std::unique_lock lock2(m_Mutex);
                                   m_SimpleEvents[event].erase(id);
                               });
    }

    /// Subscribe an event with argument.
    template<typename Arg>
    EventConnection Subscribe(EventEnum event, std::function<void(Arg&)> cb)
    {
        std::unique_lock lock(m_Mutex);
        HandlerID id = ++m_NextID;
        auto& map = m_ArgEvents<Arg>();
        map[event][id] = std::move(cb);

        return EventConnection([=]()
                               {
                                   std::unique_lock lock2(m_Mutex);
                                   map[event].erase(id);
                               });
    }

    /// Fire a simple event.
    void Fire(EventEnum event) const
    {
        std::shared_lock lock(m_Mutex);
        if(auto it = m_SimpleEvents.find(event); it != m_SimpleEvents.end())
        {
            for(const auto& [id, cb] : it->second)
                cb();
        }
    }

    /// Fire an event with argument.
    template<typename Arg>
    void Fire(EventEnum event, Arg& arg) const
    {
        std::shared_lock lock(m_Mutex);
        const auto& map = m_ArgEvents<Arg>();
        if(auto it = map.find(event); it != map.end())
        {
            for(const auto& [id, cb] : it->second)
                cb(arg);
        }
    }

private:
    using HandlerID = std::size_t;

    mutable std::shared_mutex m_Mutex;
    mutable std::atomic<HandlerID> m_NextID{ 0 };

    std::unordered_map<EventEnum, std::unordered_map<HandlerID, std::function<void()>>> m_SimpleEvents;

    template<typename Arg>
    static auto& m_ArgEvents()
    {
        static std::unordered_map<EventEnum, std::unordered_map<HandlerID, std::function<void(Arg&)>>> instance;
        return instance;
    }
};

/**
 * @brief Async event provider with safe subscribe/unsubscribe and async dispatch.
 *
 * @tparam EventEnum Enum type identifying each event.
 */
template<typename EventEnum>
class AsyncEventProvider
{
public:
    AsyncEventProvider(size_t threadCount = std::thread::hardware_concurrency())
        : m_Stop(false)
    {
        for(size_t i = 0; i < threadCount; ++i)
        {
            m_Workers.emplace_back([this]
                                   {
                                       WorkerLoop();
                                   });
        }
    }

    ~AsyncEventProvider()
    {
        {
            std::unique_lock lock(m_QueueMutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        for(auto& t : m_Workers)
        {
            if(t.joinable())
                t.join();
        }
    }

    /// Subscribe a simple event.
    EventConnection Subscribe(EventEnum event, std::function<void()> cb)
    {
        std::unique_lock lock(m_Mutex);
        HandlerID id = ++m_NextID;
        m_SimpleEvents[event][id] = std::move(cb);

        return EventConnection([=]()
                               {
                                   std::unique_lock lock2(m_Mutex);
                                   m_SimpleEvents[event].erase(id);
                               });
    }

    /// Subscribe an event with argument.
    template<typename Arg>
    EventConnection Subscribe(EventEnum event, std::function<void(Arg&)> cb)
    {
        std::unique_lock lock(m_Mutex);
        HandlerID id = ++m_NextID;
        auto& map = m_ArgEvents<Arg>();
        map[event][id] = std::move(cb);

        return EventConnection([=]()
                               {
                                   std::unique_lock lock2(m_Mutex);
                                   map[event].erase(id);
                               });
    }

    /// Fire a simple event asynchronously.
    void Fire(EventEnum event)
    {
        std::shared_lock lock(m_Mutex);
        if(auto it = m_SimpleEvents.find(event); it != m_SimpleEvents.end())
        {
            for(const auto& [id, cb] : it->second)
                Enqueue([cb] { cb(); });
        }
    }

    /// Fire an event with argument asynchronously.
    template<typename Arg>
    void Fire(EventEnum event, Arg arg)
    {
        std::shared_lock lock(m_Mutex);
        const auto& map = m_ArgEvents<Arg>();
        if(auto it = map.find(event); it != map.end())
        {
            for(const auto& [id, cb] : it->second)
            {
                Enqueue([cb, arg]() mutable { cb(arg); });
            }
        }
    }

private:
    using HandlerID = std::size_t;

    mutable std::shared_mutex m_Mutex;
    mutable std::atomic<HandlerID> m_NextID{ 0 };

    std::unordered_map<EventEnum, std::unordered_map<HandlerID, std::function<void()>>> m_SimpleEvents;

    template<typename Arg>
    static auto& m_ArgEvents()
    {
        static std::unordered_map<EventEnum, std::unordered_map<HandlerID, std::function<void(Arg&)>>> instance;
        return instance;
    }

    // --- Async worker pool ---
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_TaskQueue;
    std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    bool m_Stop = false;

    void Enqueue(std::function<void()> task)
    {
        {
            std::unique_lock lock(m_QueueMutex);
            m_TaskQueue.push(std::move(task));
        }
        m_Condition.notify_one();
    }

    void WorkerLoop()
    {
        while(true)
        {
            std::function<void()> task;

            {
                std::unique_lock lock(m_QueueMutex);
                m_Condition.wait(lock, [this]
                                 {
                                     return m_Stop || !m_TaskQueue.empty();
                                 });

                if(m_Stop && m_TaskQueue.empty())
                    return;

                task = std::move(m_TaskQueue.front());
                m_TaskQueue.pop();
            }

            if(task)
                task();
        }
    }
};
