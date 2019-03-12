/*=============================================================================
 * TarotClub - Util.h
 *=============================================================================
 * General purpose system methods
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

#ifndef ICL_UTIL_H
#define ICL_UTIL_H

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <random>

/*****************************************************************************/
class Util
{
public:
#ifdef USE_WINDOWS_OS
    static const char DIR_SEPARATOR = '\\';
#else
    static const char DIR_SEPARATOR = '/';
#endif

    static std::uint32_t CurrentTimeStamp();
    static int64_t CurrentTimeStamp64();
    static std::string TimestampToString(const std::string &format, uint32_t timestamp);
    static std::string CurrentDateTime(const std::string &format);
    static std::string ExecutablePath();
    static std::string GetCurrentDirectory();
    static std::string HomePath();
    static bool FolderExists(const std::string &foldername);
    static bool FileExists(const std::string &fileName);
    static bool Mkdir(const std::string &fullPath);
    static void ReplaceCharacter(std::string &theString, const std::string &toFind, const std::string &toReplace);
    static std::string EscapeChar(const std::string &str);
    static std::vector<std::string> Split(const std::string &theString, const std::string &delimiter);
    static std::string Join(const std::vector<std::string> &tokens, const std::string &delimiter);
    static std::int32_t GetCurrentMemoryUsage();
    static std::int32_t GetMaximumMemoryUsage();
    static std::string GetFileName(const std::string &path);
    static std::string GetFileExtension(const std::string& FileName);
    static std::string GetDirectoryPath(const std::string &path);
    static std::uint64_t FileSize(const std::string &fileName);
    static std::string ToUpper(const std::string &input);
    static inline bool EndsWith(std::string const & value, std::string const & ending)
    {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }
    // Case insensitive version
    static bool Compare(const std::string &a, const std::string &b);
    static std::wstring ToWString(const std::string &str);
    static std::string ToString(const std::wstring &wstr);
    static std::string HexDump(const char *desc, const void *addr, int len);
    static std::string GetModifiedFileDateTime(const std::string &fileName);
    static std::string ToLeadingZeros(const int value, const int precision);
    static uint32_t Exec(std::string exePath, std::string params, std::string& ListStdOut, std::string& ListStdErr, int32_t& RetCode);

    template<typename T>
    static T GenerateRandom(T min, T max)
    {
        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<T> dist(min, max);  //(min, max)

        //get one
        return dist(rng);
    }

    template<typename T>
    static std::string ToString(const T& v)
    {
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }

    template<typename T>
    static T FromString(const std::string& str)
    {
        std::istringstream ss(str);
        T ret;
        ss >> ret;
        return ret;
    }

    static std::string GenerateRandomString(uint32_t length);

};

#endif // ICL_UTIL_H

//=============================================================================
// End of file Util.h
//=============================================================================
