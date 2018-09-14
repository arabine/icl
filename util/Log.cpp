/*=============================================================================
 * TarotClub - Log.cpp
 *=============================================================================
 * Log utility class: store events and data in a chronological way
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

// C++ / STL
#include <iostream>
#include <fstream>
#include <sstream>

// TarotClub
#include "Log.h"
#include "Util.h"

std::mutex Log::mMutex;
Subject<Log::Infos> Log::mSubject;
std::string Log::mLogPath;
std::string Log::mLogFileName;
std::vector<Log::Infos> Log::mHistory;

bool Log::mEnableFileOutput = true;
bool Log::mEnableSourceInfo = true;
bool Log::mEnableHistory = false;


const std::uint8_t Log::Error   = 1U;
const std::uint8_t Log::Info    = 2U;
const std::uint8_t Log::Network = 4U;
const std::uint8_t Log::Script  = 8U;
const std::uint8_t Log::Server  = 16U;
const std::uint8_t Log::Warning  = 32U;
const std::uint8_t Log::All     = Log::Error | Log::Info | Log::Network | Log::Script | Log::Server | Log::Warning;

static std::map<std::uint8_t, std::string> LogInit();
static std::map<std::uint8_t, std::string> eventString = LogInit();

/*****************************************************************************/
/**
 * @brief One time initialization of a global variable
 * @return
 */
std::map<uint8_t, std::string> LogInit()
{
    std::map<std::uint8_t, std::string> evt;

    evt[Log::Error]      = "Error";
    evt[Log::Info]       = "Info";
    evt[Log::Network]    = "Network";
    evt[Log::Script]     = "Script";
    evt[Log::Server]     = "Server";
    evt[Log::Warning]    = "Warning";

    return evt;
}
/*****************************************************************************/
void Log::RegisterListener(Observer<Infos> &listener)
{
    mMutex.lock();
    mSubject.Attach(listener);
    mMutex.unlock();
}
/*****************************************************************************/
void Log::RemoveListener(Observer<Infos> &listener)
{
    mMutex.lock();
    mSubject.Detach(listener);
    mMutex.unlock();
}
/*****************************************************************************/
void Log::SetLogPath(const std::string &path)
{
    mLogPath = path;
}
/*****************************************************************************/
void Log::SetLogFileName(const std::string &fileName)
{
    mLogFileName = fileName;
}
/*****************************************************************************/
std::string Log::GetLogFileName()
{
    return mLogFileName;
}
/*****************************************************************************/
void Log::ClearHistory()
{
    mHistory.clear();
}
/*****************************************************************************/
std::vector<Log::Infos> Log::GetHistory()
{
    return mHistory;
}
/*****************************************************************************/
void Log::Clear()
{
    mMutex.lock();
    mSubject.Clear();
    mMutex.unlock();
}
/*****************************************************************************/
void Log::AddEntry(uint8_t event, const std::string &file, const int line, const std::string &message)
{
    Infos infos;
    infos.event = event;
    infos.file = file;
    infos.line = line;
    infos.message = message;

    mMutex.lock();
    mSubject.Notify(infos, event);    // send message to all the listeners
    mMutex.unlock();

    if (mEnableHistory)
    {
        mHistory.push_back(infos);
    }

    if (mEnableFileOutput)
    {
        Save(infos.ToString());                     // save message to a file
    }
}
/*****************************************************************************/
void Log::Save(const std::string &line)
{
    std::fstream f;
    std::string fileName;

    Util::Mkdir(mLogPath); // Make sure path exists

    if (mLogFileName.size() > 0)
    {
        // Use specified file name
        fileName = mLogFileName;
    }
    else
    {
        // One log file per day should be enough!
        fileName = "log_" + Util::CurrentDateTime("%Y-%m-%d") + ".csv";
    }

    fileName = mLogPath + Util::DIR_SEPARATOR + fileName;

    mMutex.lock();

    // Avoid generating too much log for the server
    if (Util::FileSize(fileName) < SizeLimit)
    {
        f.open(fileName, std::ios_base::out | std::ios_base::binary  | std::ios_base::app);

        if (f.is_open())
        {
            f << line << std::endl;
            f.close();
        }
    }
    mMutex.unlock();
}
/*****************************************************************************/
std::string Log::Infos::ToString() const
{
    std::stringstream ss;

    ss << eventString[event] << ", " << Util::CurrentDateTime("%Y-%m-%d.%X") << ", ";

    if (Log::mEnableSourceInfo)
    {
        ss <<  file << ", " << line << ", ";
    }

    ss << message;

    return ss.str();
}

//=============================================================================
// End of file Log.cpp
//=============================================================================

