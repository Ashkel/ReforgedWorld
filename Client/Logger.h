#pragma once

#include <Core/Utility/AsyncLogger.hpp>

class Logger : public AsyncLogger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error,
        Debug,
    };

    explicit Logger(const std::string& logFile)
        : AsyncLogger(logFile)
    {
        Log(Level::Info, "Logger initialized.");
    }

    ~Logger()
    {
        Log(Level::Info, "Logger shutting down.");
    }

    /**
     * @brief Log a message with log level.
     */
    template<typename... Args>
    std::shared_future<void> Log(Level level, Args&&... args)
    {
        return AsyncLogger::Log("[" + LevelToString(level) + "]", std::forward<Args>(args)...);
    }

    /**
     * @brief Log with file + line context.
     */
    template<typename... Args>
    std::shared_future<void> Log(Level level, const char* file, int line, Args&&... args)
    {
        std::filesystem::path filepath(file);
        std::string filename = filepath.filename().string();
        return AsyncLogger::Log(
            "[" + LevelToString(level) + "]",
            filename + ":" + std::to_string(line) + ":",
            std::forward<Args>(args)...
        );
    }

private:
    static std::string LevelToString(Level level)
    {
        switch(level)
        {
            case Level::Info:    return "INFO";
            case Level::Warning: return "WARNING";
            case Level::Error:   return "ERROR";
            case Level::Debug:   return "DEBUG";
            default:             return "UNKNOWN";
        }
    }
};

#pragma once

#define LOG_INFO(logger, ...)    logger.Log(GameLogger::Level::Info,    __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(logger, ...) logger.Log(GameLogger::Level::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(logger, ...)   logger.Log(GameLogger::Level::Error,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(logger, ...)   logger.Log(GameLogger::Level::Debug,   __FILE__, __LINE__, __VA_ARGS__)
