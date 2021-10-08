/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef ICL_UTIL_H
#define ICL_UTIL_H

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <random>
#include <chrono>

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
    static std::string TimestampToString(time_t timestamp, const std::string &format);
    static std::string CurrentDateTime(const std::string &format);
    static std::string ToISODateTime(const std::chrono::system_clock::time_point &tp);
    static std::chrono::system_clock::time_point FromISODateTimeFormat(const std::string &str, const std::string &format);
    static std::chrono::system_clock::time_point FromISODateTime(const std::string &str);
    static std::chrono::system_clock::time_point FromISODate(const std::string &str);
    static std::string DateTimeFormat(const std::chrono::system_clock::time_point &tp, const std::string &format);
    static int GetYear(const std::chrono::system_clock::time_point &tp);

    static std::string ExecutablePath();
    static std::string GetWorkingDirectory();
    static std::string HomePath();
    static bool FolderExists(const std::string &foldername);
    static bool FileExists(const std::string &fileName);
    static bool Mkdir(const std::string &fullPath);
    static void ReplaceCharacter(std::string &theString, const std::string &toFind, const std::string &toReplace);
    static void EraseString(std::string &theString, const std::string &toErase);
    static void Ltrim(std::string &s);
    static void Rtrim(std::string &s);
    static void Trim(std::string &s);
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
    static std::string ToLower(const std::string &text);
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

    static bool Contains(const std::string &str, const std::string &lookfor);
    static std::string Match(const std::string &msg, const std::string &patternString);

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

    static void HexStringToUint8(const std::string_view &input, uint8_t *output);
    static std::string GenerateRandomString(uint32_t length);
    static void ByteToHex(const char byte, char *out);
    static std::string ToHex(const char *buf, size_t size);
    static std::string FileToString(const std::string &filePath);
    static bool StringToFile(const std::string &filePath, const std::string &data, bool makeExecutable = false);
    static bool IsDigitOrAlpha(const std::string &s);
    static int32_t GetRandom(int32_t from, int32_t to);
    static bool ExecWithFork(const std::string &cmd);
    static int GetMacAddress(const char *ifname, unsigned char chMAC[]);
    static std::string GetIpAddress(const char *ifname);
    static std::vector<std::string> GetDirectoryFiles(const std::string& dir, const std::string &extension);
    static std::vector<std::string> ListFiles(const std::string &dir, const std::string &expression);
};

#endif // ICL_UTIL_H

//=============================================================================
// End of file Util.h
//=============================================================================
