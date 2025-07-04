#pragma once

#if defined(_DEBUG) && defined(_WIN32)

#include <windows.h>
#include <cstdio>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>

/**
 * @brief A debug console for Windows GUI apps.
 * Opens a console window and listens for interactive debug commands.
 */
class DebugConsole
{
public:
    /// Type for user-defined command handlers.
    using CommandHandler = std::function<void()>;

    /// Creates and attaches the console.
    DebugConsole()
        : m_Running(true)
    {
        if(AllocConsole())
        {
            freopen_s(&m_fpOut, "CONOUT$", "w", stdout);
            freopen_s(&m_fpErr, "CONOUT$", "w", stderr);
            freopen_s(&m_fpIn, "CONIN$", "r", stdin);

            std::cout << "Debug console started. Type 'help' for commands.\n";

            // Register built-in commands
            m_Commands["exit"] = [this]()
                {
                    std::cout << "Shutting down debug console.\n";
                    m_Running = false;
                };

            m_Commands["help"] = [this]()
                {
                    std::cout << "Available commands:\n";
                    for(const auto& [cmd, _] : m_Commands)
                    {
                        std::cout << " - " << cmd << "\n";
                    }
                };

                // Start command listener thread
            m_Thread = std::thread([this]() { ListenLoop(); });
        }
    }

    /// Cleans up and closes the console.
    ~DebugConsole()
    {
        m_Running = false;

        if(m_Thread.joinable())
            m_Thread.join();

        if(m_fpOut) fclose(m_fpOut);
        if(m_fpErr) fclose(m_fpErr);
        if(m_fpIn)  fclose(m_fpIn);

        FreeConsole();
    }

    /// Register a custom command.
    void RegisterCommand(const std::string& name, CommandHandler handler)
    {
        m_Commands[name] = std::move(handler);
    }

private:
    FILE* m_fpOut = nullptr;
    FILE* m_fpErr = nullptr;
    FILE* m_fpIn = nullptr;

    std::atomic<bool> m_Running;
    std::thread m_Thread;

    std::unordered_map<std::string, CommandHandler> m_Commands;

    void ListenLoop()
    {
        std::string line;

        while(m_Running)
        {
            std::cout << "> ";
            if(!std::getline(std::cin, line))
                break;

            auto cmd = Trim(line);
            if(cmd.empty())
                continue;

            auto it = m_Commands.find(cmd);
            if(it != m_Commands.end())
            {
                it->second();
            }
            else
            {
                std::cout << "Unknown command: '" << cmd << "'\n";
            }
        }
    }

    /// Trim whitespace from both ends.
    static std::string Trim(const std::string& str)
    {
        const auto start = str.find_first_not_of(" \t\r\n");
        if(start == std::string::npos) return "";
        const auto end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

#endif // _DEBUG && _WIN32
