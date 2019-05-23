/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef LOG_H
#define LOG_H

#include <cstdint>
#include <mutex>
#include <map>
#include <vector>
#include "Observer.h"

/*****************************************************************************/
/**
 * @brief The Log class
 *
 * File format is CSV, each new action will append a new entry at the end of the file.
 *
 * "Category", "Date", "File", "Message"
 *
 * Example:
 * Error, 19-04-2013 09:17:30, "Protocol.cpp", "Buffer length too small!",
 *
 */

class Log
{
public:
    static const std::uint8_t Error;
    static const std::uint8_t Info;
    static const std::uint8_t Network;
    static const std::uint8_t Script;
    static const std::uint8_t Server;
    static const std::uint8_t Warning;
    static const std::uint8_t All;

    static const std::uint32_t SizeLimit = 50 * (1024U * 1024U); // Filesize limit to 50MB per log

    struct Infos
    {
        std::uint8_t event;
        std::string file;
        int line;
        std::string message;

        std::string ToString() const;
    };

    static void AddEntry(std::uint8_t event, const std::string &file, const int line, const std::string &message);
    static void RegisterListener(Observer<Infos> &listener);
    static void RemoveListener(Observer<Infos> &listener);
    static void SetLogPath(const std::string &path);
    static void SetLogFileName(const std::string &fileName);
    static std::string GetLogFileName();
    static void ClearHistory();
    static std::vector<Log::Infos> GetHistory();
    static void EnableSourceInfos(bool enable) { mEnableSourceInfo = enable; }
    static void Clear();
    static void EnableLog(bool enable) { mEnableFileOutput = enable; }

private:
    static void Save(const std::string &line);

    static std::mutex mMutex;
    static Subject<Log::Infos> mSubject;
    static std::string mLogPath;
    static std::string mLogFileName;
    static std::vector<Log::Infos> mHistory;
    static bool mEnableFileOutput;
    static bool mEnableSourceInfo;
    static bool mEnableHistory;
};

// Macros definitions
#define TLogInfo(message)       Log::AddEntry(Log::Info, __FILE__, __LINE__, (message))
#define TLogWarning(message)    Log::AddEntry(Log::Warning, __FILE__, __LINE__, (message))
#define TLogError(message)      Log::AddEntry(Log::Error, __FILE__, __LINE__, (message))
#define TLogNetwork(message)    Log::AddEntry(Log::Network, __FILE__, __LINE__, (message))
#define TLogScript(message)     Log::AddEntry(Log::Script, __FILE__, __LINE__, (message))
#define TLogServer(message)     Log::AddEntry(Log::Server, __FILE__, __LINE__, (message))
#define TLogPrint(message)      Log::Print(message)

#endif // LOG_H

//=============================================================================
// End of file Log.h
//=============================================================================
