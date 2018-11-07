/*=============================================================================
 * TarotClub - Util.cpp
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

#ifdef USE_WINDOWS_OS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <direct.h>
#include <sys/stat.h>
#include <windows.h>
#include <psapi.h>

static const HANDLE WIN_INVALID_HND_VALUE = reinterpret_cast<HANDLE>(0xFFFFFFFFUL);

#define popen _popen
#define pclose _pclose

#endif

#ifdef USE_UNIX_OS
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/resource.h>
#endif

#ifdef USE_APPLE_OS
#include <mach/mach.h>
#endif

#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cctype>
#include <chrono>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <thread>
#include "date.h"
#include "tz.h"
#include "Util.h"

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <typename Facet>
struct deletable_facet : Facet
{
    using Facet::Facet;
};

/*****************************************************************************/
/**
 * @brief Util::CurrentDateTime
 *
 * Gets current date/time, format is given in parameters
 * Example: "%Y-%m-%d.%X" generates the following output: 2014-03-21.18:20:49
 *
 * @param format
 * @return
 */
std::string Util::CurrentDateTime(const std::string &format)
{
    std::stringstream ss;
    auto time = std::time(nullptr);
    ss << std::put_time(std::localtime(&time), format.c_str());

    return ss.str();
}
/*****************************************************************************/
std::string Util::ExecutablePath()
{
    std::string path;

#ifdef USE_WINDOWS_OS
    wchar_t buf[MAX_PATH];

    // Will contain exe path
    HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        // When passing NULL to GetModuleHandle, it returns handle of exe itself
        GetModuleFileName(hModule, buf, MAX_PATH);
    }
    std::wstring wstr(buf);
    path = std::string(wstr.begin(), wstr.end());

#elif defined(USE_UNIX_OS)
    char buf[FILENAME_MAX];
    readlink("/proc/self/exe", buf, sizeof(buf));
    path = buf;
#elif defined(USE_APPLE_OS)
    _NSGetExecutablePath(path, &size); // make it compile
#else
#error "A portable code is needed here"
#endif
    return (GetDirectoryPath(path));
}
/*****************************************************************************/
std::string Util::HomePath()
{
    std::string homedir;

#if defined(USE_WINDOWS_OS)

#if defined(_MSC_VER)
	char* buf = 0;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, "HOMEDRIVE") == 0)
	{
		homedir.append(buf);
		free(buf);
	}
	if (_dupenv_s(&buf, &sz, "HOMEPATH") == 0)
	{
		homedir.append(buf);
		free(buf);
	}
#else
    homedir = getenv("HOMEDRIVE");
    homedir += getenv("HOMEPATH");
#endif
#else
    homedir = getenv("HOME");
#endif
    return homedir;
}
/*****************************************************************************/
/**
 * Checks if a folder exists
 * @param foldername path to the folder to check.
 * @return true if the folder exists, false otherwise.
 */
bool Util::FolderExists(const std::string &foldername)
{
    bool ret = false;
    struct stat st;
    if (::stat(foldername.c_str(), &st) == 0)
    {
        if ((st.st_mode & S_IFDIR) != 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
/**
 * Checks if a file exists
 * @param filename of the file to check.
 * @return true if the file exists, false otherwise.
 */
bool Util::FileExists(const std::string &fileName)
{
    bool ret = false;
    struct stat st;
    if (::stat(fileName.c_str(), &st) == 0)
    {
        if ((st.st_mode & S_IFREG) != 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
/**
 * @brief Util::FileSize
 * @param fileName
 * @return -1 means file not found
 */
std::int64_t Util::FileSize(const std::string &fileName)
{
    std::int64_t size = -1;
    struct stat st;
    if (::stat(fileName.c_str(), &st) == 0)
    {
        size = st.st_size;
    }
    return size;
}
/*****************************************************************************/
std::string Util::ToUpper(const std::string &input)
{
    std::string str = input;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}
/*****************************************************************************/
std::string Util::GetFileName(const std::string &path)
{
    unsigned found = path.find_last_of("/\\");
    return path.substr(found+1);
}
/*****************************************************************************/
std::string Util::GetDirectoryPath(const std::string &path)
{
    unsigned found = path.find_last_of("/\\");
    return path.substr(0,found);
}
/*****************************************************************************/
std::string Util::GetModifiedFileDateTime(const std::string &fileName)
{
    struct stat result;
    std::string dateTime;

    if (::stat(fileName.c_str(), &result)==0)
    {
        struct tm * timeinfo = std::localtime(&result.st_mtime); // or gmtime() depending on what you want
        dateTime = asctime(timeinfo);
    }

    return dateTime;
}
/*****************************************************************************/
std::string Util::ToLeadingZeros(const int value, const int precision)
{
    std::ostringstream oss;
    oss << std::setw(precision) << std::setfill('0') << value;
    return oss.str();
}
/*****************************************************************************/
std::uint32_t Util::Exec(
        std::string     exePath,    //Command Line
        std::string     params,  //set to '.' for current directory
        std::string&    ListStdOut, //Return List of StdOut
        std::string&    ListStdErr, //Return List of StdErr
        int32_t&        RetCode)    //Return Exit Code
{
    std::uint32_t status = 0;
#ifdef USE_WINDOWS_OS

    int                  Success;
    SECURITY_ATTRIBUTES  security_attributes;
    HANDLE               stdout_rd = WIN_INVALID_HND_VALUE;
    HANDLE               stdout_wr = WIN_INVALID_HND_VALUE;
    HANDLE               stderr_rd = WIN_INVALID_HND_VALUE;
    HANDLE               stderr_wr = WIN_INVALID_HND_VALUE;
    PROCESS_INFORMATION  process_info;
    STARTUPINFO          startup_info;
    std::thread               stdout_thread;
    std::thread               stderr_thread;

    security_attributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
    security_attributes.bInheritHandle       = TRUE;
    security_attributes.lpSecurityDescriptor = nullptr;

    if (!CreatePipe(&stdout_rd, &stdout_wr, &security_attributes, 0) ||
            !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0))
    {
        status = GetLastError();
        return status;
    }

    if (!CreatePipe(&stderr_rd, &stderr_wr, &security_attributes, 0) ||
            !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0))
    {
        if (stdout_rd != WIN_INVALID_HND_VALUE) CloseHandle(stdout_rd);
        if (stdout_wr != WIN_INVALID_HND_VALUE) CloseHandle(stdout_wr);
        status = GetLastError();
        return status;
    }

    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&startup_info, sizeof(STARTUPINFO));

    startup_info.cb         = sizeof(STARTUPINFO);
    startup_info.hStdInput  = nullptr;
    startup_info.hStdOutput = stdout_wr;
    startup_info.hStdError  = stderr_wr;

    if(stdout_rd || stderr_rd)
    {
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
    }

    std::wstring FullPathToExe = L"\"" + Util::ToWString(exePath) +  L"\"";
    FullPathToExe += L' ' + Util::ToWString(params);

    std::wstring sTempStr = L"";

     /* CreateProcessW can modify Parameters thus we allocate needed memory */
    wchar_t * pwszParam = new wchar_t[FullPathToExe.size() + 1];
    if (pwszParam == nullptr)
    {
        return 1;
    }
    const wchar_t* pchrTemp = FullPathToExe.c_str();
    wcscpy_s(pwszParam, FullPathToExe.size() + 1, pchrTemp);

    Success = CreateProcess(
        nullptr,
        pwszParam,
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &startup_info,
        &process_info
    );
    CloseHandle(stdout_wr);
    CloseHandle(stderr_wr);

    if(!Success)
    {
        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);
        CloseHandle(stdout_rd);
        CloseHandle(stderr_rd);
        status = GetLastError();
    }
    else
    {
        CloseHandle(process_info.hThread);

        if(stdout_rd)
        {
            stdout_thread = std::thread([&]()
            {
                DWORD  n;
                const size_t bufsize = 1000;
                char         buffer [bufsize];
                for(;;) {
                    n = 0;
                    int Success = ReadFile(
                        stdout_rd,
                        buffer,
                        static_cast<DWORD>(bufsize),
                        &n,
                        nullptr
                    );
    //                printf("STDERR: Success:%d n:%d\n", Success, static_cast<int>(n));
                    if(!Success || n == 0)
                        break;
                    std::string s(buffer, n);
    //                printf("STDOUT:(%s)\n", s.c_str());
                    ListStdOut += s;
                }
    //            printf("STDOUT:BREAK!\n");
            });
        }

        if(stderr_rd)
        {
            stderr_thread=std::thread([&]()
            {
                DWORD        n;
                const size_t bufsize = 1000;
                char         buffer [bufsize];
                for(;;) {
                    n = 0;
                    int Success = ReadFile(
                        stderr_rd,
                        buffer,
                        static_cast<DWORD>(bufsize),
                        &n,
                        nullptr
                    );
    //                printf("STDERR: Success:%d n:%d\n", Success, static_cast<int>(n));
                    if(!Success || n == 0)
                        break;
                    std::string s(buffer, n);
    //                printf("STDERR:(%s)\n", s.c_str());
                    ListStdErr += s;
                }
    //            printf("STDERR:BREAK!\n");
            });
        }
    }

    WaitForSingleObject(process_info.hProcess, INFINITE);
    if(!GetExitCodeProcess(process_info.hProcess, reinterpret_cast<DWORD*>(&RetCode)))
    {
         status = GetLastError();
    }

    /* Free memory */
    delete[]pwszParam;
    pwszParam = nullptr;

    CloseHandle(process_info.hProcess);

    if(stdout_thread.joinable())
    {
        stdout_thread.join();
    }

    if(stderr_thread.joinable())
    {
        stderr_thread.join();
    }

    if (stdout_rd)
    {
        CloseHandle(stdout_rd);
    }
    if (stderr_rd)
    {
        CloseHandle(stderr_rd);
    }

    return status;

#else

    std::array<char, 128> buffer;

    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (pipe)
    {
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                ListStdOut += buffer.data();
        }
    }
    else
    {
        std::cout << "PIPE open failed!" << std::endl;
    }

    return 0
#endif
}
/*****************************************************************************/
/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int my_mkdir(const char *path)
{
#if defined(USE_WINDOWS_OS)
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0755); // not sure if this works on mac
#endif
}
/*****************************************************************************/
/**
 * Recursive, portable wrapper for mkdir.
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
bool Util::Mkdir(const std::string &fullPath)
{
    bool ret = true;
    std::string temp = fullPath;

#ifdef USE_WINDOWS_OS
    ReplaceCharacter(temp, "/", "\\");
#endif

    std::string current_level = "";
    std::string level;
    std::stringstream ss(temp);

    // split path using slash as a separator
    while (std::getline(ss, level, Util::DIR_SEPARATOR))
    {
        current_level += level; // append folder to the current level

        if (!FolderExists(current_level))
        {
#ifdef USE_WINDOWS_OS
            if ((current_level.size() == 2) &&
                    (current_level[1] == ':'))
            {
                // Do nothing, skip disk name
            }
            else
#endif
                if (current_level.size() > 2)
                {
                    // create current level
                    if (my_mkdir(current_level.c_str()) != 0)
                    {
                        ret = false;
                        break;
                    }
                }
        }
        current_level += Util::DIR_SEPARATOR; // don't forget to append a slash
    }

    return ret;
}
/*****************************************************************************/
void Util::ReplaceCharacter(std::string &theString, const std::string &toFind, const std::string &toReplace)
{
    std::size_t found;
    do
    {
        found = theString.find(toFind);
        if (found != std::string::npos)
        {
            theString.replace(found, 1, toReplace);
        }
    }
    while (found != std::string::npos);
}
/*****************************************************************************/
std::vector<std::string> Util::Split(const std::string &theString, const std::string &delimiter)
{
    std::vector<std::string> theStringVector;
    size_t  start = 0, end = 0;

    while (end != std::string::npos)
    {
        end = theString.find(delimiter, start);

        // If at end, use length=maxLength.  Else use length=end-start.
        theStringVector.push_back(theString.substr(start,
                                  (end == std::string::npos) ? std::string::npos : end - start));

        // If at end, use start=maxSize.  Else use start=end+delimiter.
        start = ((end > (std::string::npos - delimiter.size()))
                 ?  std::string::npos  :  end + delimiter.size());
    }
    return theStringVector;
}
/*****************************************************************************/
std::string Util::EscapeChar(const std::string &str)
{
    std::string escaped = str;
    ReplaceCharacter(escaped, "\\", "/");
    ReplaceCharacter(escaped, "\r\n", "\\n");
    ReplaceCharacter(escaped, "\n", "\\n");
    ReplaceCharacter(escaped, "\t", "\\t");

    return escaped;
}
/*****************************************************************************/
std::string Util::Join(const std::vector<std::string> &tokens, const std::string &delimiter)
{
    std::stringstream ss;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        // Add the delimiter between tokens only, nothing at the end of the string
        if (i != 0)
        {
            ss << delimiter;
        }
        ss << tokens[i];
    }

    return ss.str();
}
/*****************************************************************************/
static bool icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

bool Util::Compare(std::string const& a, std::string const& b)
{
    if (a.length()==b.length()) {
        return std::equal(b.begin(), b.end(),
                           a.begin(), icompare_pred);
    }
    else {
        return false;
    }
}
/*****************************************************************************/
std::wstring Util::ToWString(const std::string &str)
{
    std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> conv;

    return conv.from_bytes(str);
}
/*****************************************************************************/
std::string Util::ToString(const std::wstring &wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
/*****************************************************************************/
/**
 * @brief Util::GetCurrentMemoryUsage
 *
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 *
 */
std::int32_t Util::GetCurrentMemoryUsage()
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return static_cast<std::int32_t>(info.WorkingSetSize);

#elif defined(__APPLE__) && defined(__MACH__)
    /* OSX ------------------------------------------------------ */
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,
        (task_info_t)&info, &infoCount ) != KERN_SUCCESS )
        return (size_t)0L;      /* Can't access? */
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    /* Linux ---------------------------------------------------- */
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);

#else
    /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
    return (size_t)0L;          /* Unsupported. */
#endif
}
/*****************************************************************************/
/**
 * @brief GetMaximumMemoryUsage
 *
 * Returns the peak (maximum so far) resident set size (physical memory use)
 * measured in bytes, or zero if the value cannot be determined on this OS.
 */
std::int32_t Util::GetMaximumMemoryUsage()
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return static_cast<std::int32_t>(info.PeakWorkingSetSize);

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    /* AIX and Solaris ------------------------------------------ */
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* BSD, Linux, and OSX -------------------------------------- */
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage );
#if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif

#else
    /* Unknown OS ----------------------------------------------- */
    return (size_t)0L;          /* Unsupported. */
#endif
}
/*****************************************************************************/
std::string Util::HexDump(const char *desc, const void *addr, int len)
{
    int i;
    unsigned char buff[17];
    const unsigned char *pc = static_cast<const unsigned char*>(addr);
    std::stringstream ss;

    // Output description if given.
    if (desc != nullptr)
    {
        ss << desc << ":\n";
    }

    if (len == 0) {
        ss << "  ZERO LENGTH\n";
        return ss.str();
    }
    if (len < 0) {
        ss << "  NEGATIVE LENGTH: " << len << "\n";
        return ss.str();
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                ss <<  "  " << buff << "\n";

            // Output the offset.
            ss <<  " " << std::setfill('0') << std::setw(4) << std::hex  << i;
        }

        // Now the hex code for the specific character.
        ss <<  " " << std::setfill('0') << std::setw(2) << std::hex  << static_cast<int>(pc[i]) << ", ";

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        ss <<  "   ";
        i++;
    }

    // And print the final ASCII bit.
    ss << "  "<< buff << "\n";
    return ss.str();
}

//=============================================================================
// End of file Util.cpp
//=============================================================================
