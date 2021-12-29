/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifdef USE_WINDOWS_OS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <direct.h>
#include <sys/stat.h>

#ifdef _MSC_VER

#include <Windows.h>
#include <Psapi.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#else
// MINGW


#include <windows.h>
#include <psapi.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <winsock2.h>
#endif


static const HANDLE WIN_INVALID_HND_VALUE = reinterpret_cast<HANDLE>(-1);

#define popen _popen
#define pclose _pclose

#endif

#ifdef USE_LINUX_OS
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#ifdef USE_APPLE_OS
#include <mach/mach.h>
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#endif

#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <cctype>
#include <chrono>
#include <locale>
#include <codecvt>
#include <array>
#include <thread>
#include <regex>
#include <random>
#include <memory>
#include <filesystem>
//#include "date.h"
//#include "tz.h"
#include "Util.h"


// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <typename Facet>
struct deletable_facet : Facet
{
    using Facet::Facet;
};

/*****************************************************************************/
std::string Util::GenerateRandomString(uint32_t length)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz*$=+()_-[]#~&;:,!?{}@";
    std::string rstr;

    std::mt19937 eng(time(nullptr));

    std::uniform_int_distribution<int> uniform_int(0, (sizeof(alphanum) - 1));

    for (uint32_t i = 0; i < length; ++i)
    {
        rstr.push_back(alphanum[uniform_int(eng)]);
    }
    return rstr;
}
/*****************************************************************************/
int32_t Util::GetRandom(int32_t from, int32_t to)
{
    std::mt19937 eng(time(nullptr));

    std::uniform_int_distribution<int32_t> uniform_int(from, to);

    return uniform_int(eng);
}
/*****************************************************************************/
uint32_t Util::CurrentTimeStamp()
{
    std::chrono::seconds epoch = std::chrono::duration_cast< std::chrono::seconds >(
        std::chrono::system_clock::now().time_since_epoch()
    );

    return static_cast<std::uint32_t>(epoch.count());
}
/*****************************************************************************/
int64_t Util::CurrentTimeStamp64()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
}
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
    std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

    return Util::DateTimeFormat(p, format);
}
/*****************************************************************************/
std::string Util::ToISODateTime(const std::chrono::system_clock::time_point &tp)
{
    return Util::DateTimeFormat(tp, "%Y-%m-%dT%H:%M:%S");
}
/*****************************************************************************/
std::chrono::system_clock::time_point Util::FromISODateTime(const std::string &str)
{
    return Util::FromISODateTimeFormat(str, "%Y-%m-%dT%H:%M:%S");
}
/*****************************************************************************/
std::chrono::system_clock::time_point Util::FromISODate(const std::string &str)
{
    return Util::FromISODateTimeFormat(str, "%Y-%m-%d");
}
/*****************************************************************************/
std::chrono::system_clock::time_point Util::FromISODateTimeFormat(const std::string &str, const std::string &format)
{
    std::istringstream iss{str};
    std::tm tm{};
    if (!(iss >> std::get_time(&tm, format.c_str()))) {
        return std::chrono::system_clock::now();
    }

    std::chrono::system_clock::time_point timePoint{std::chrono::seconds(std::mktime(&tm))};
    if (iss.eof())
       return timePoint;
    double zz;
    if (iss.peek() != '.' || !(iss >> zz)){
        return std::chrono::system_clock::now();
    }
    using sys_clock = std::chrono::system_clock;
    std::size_t zeconds = zz * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num;
    timePoint += sys_clock::duration(zeconds);
    return timePoint;
}
/*****************************************************************************/
std::string Util::DateTimeFormat(const std::chrono::system_clock::time_point &tp, const std::string &format)
{
    return Util::TimestampToString(std::chrono::system_clock::to_time_t(tp), format);
}
/*****************************************************************************/
int Util::GetYear(const std::chrono::system_clock::time_point &tp)
{
    time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    tm local_tm = *std::localtime(&timestamp);
    return local_tm.tm_year + 1900;
}
/*****************************************************************************/
std::string Util::TimestampToString(time_t timestamp, const std::string &format)
{
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), format.c_str());

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

#elif defined(USE_LINUX_OS)
    char buf[FILENAME_MAX];
    (void) readlink("/proc/self/exe", buf, sizeof(buf));
    path = buf;
#elif defined(USE_APPLE_OS)
    char buf[FILENAME_MAX];
    uint32_t size = sizeof(buf);
    _NSGetExecutablePath(buf, &size); // make it compile
    path = buf;
#else
#error "A portable code is needed here"
#endif
    return (GetDirectoryPath(path));
}
/*****************************************************************************/
std::string Util::Util::GetWorkingDirectory()
{
    std::string currentDir;
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
       currentDir.append(cwd);
    }
    return currentDir;
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
std::uint64_t Util::FileSize(const std::string &fileName)
{
    std::uint64_t size = 0;
    struct stat st;
    if (::stat(fileName.c_str(), &st) == 0)
    {
        size = static_cast<std::uint64_t>(st.st_size);
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
std::string Util::ToLower(const std::string &input)
{
    std::string str = input;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}
/*****************************************************************************/
std::string Util::GetFileName(const std::string &path)
{
    auto found = path.find_last_of("/\\");
    return path.substr(found+1);
}
/*****************************************************************************/
std::string Util::GetFileExtension(const std::string &FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}
/*****************************************************************************/
std::string Util::GetDirectoryPath(const std::string &path)
{
    auto found = path.find_last_of("/\\");
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

#ifdef USE_WINDOWS_OS

     std::uint32_t status = 0;

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

    (void) ListStdErr; // FIXME: update this on linux
    RetCode = 0; // FIXME: update this on linux

    std::array<char, 128> buffer;

    std::string cmd = exePath + " " + params + " 2>&1";

    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (pipe)
    {
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                ListStdOut += buffer.data();
        }
    }

    return 0;
#endif
}
/*****************************************************************************/
bool Util::ExecWithFork(const std::string &cmd)
{
    bool success = false;
#ifdef USE_LINUX_OS
    int pid = fork();
    if (pid == 0)
    {
        // We are in the child process, execute the command
        execl(cmd.c_str(), cmd.c_str(), nullptr);
//        system(cmd.c_str());
        // Exit child process
        exit(1);
    }
    else if (pid > 0)
    {
        // The parent process, do whatever is needed
        // The parent process can even exit while the child process is running, since it's independent
        success = true;
    }
#endif
    return success;
}
/*****************************************************************************/
bool Util::Contains(const std::string &str, const std::string &lookfor)
{
    return str.find(lookfor) != std::string::npos;
}
/*****************************************************************************/
std::string Util::Match(const std::string &msg, const std::string &patternString)
{
    std::regex pattern(patternString);
    std::smatch matcher;
    std::string subMatch;

    std::regex_search(msg, matcher, pattern);

    if (matcher.size() == 2)
    {
        // Extracted value is located at index 1
        subMatch = matcher[1].str();
    }
    return subMatch; // empty string if not found
}
/*****************************************************************************/
void Util::HexStringToUint8(const std::string_view &input, uint8_t *output)
{
    for(size_t i = 0; i < input.length(); i += 2)
    {
        std::stringstream converter;
        converter << std::hex << input.substr(i,2);
        int byte;
        converter >> byte;
        output[i/2] = byte & 0xFF;
    }
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
void Util::EraseString(std::string &theString, const std::string &toErase)
{
    std::size_t found;
    found = theString.find(toErase);
    if (found != std::string::npos)
    {
        theString.erase(found, toErase.size());
    }
}
/*****************************************************************************/
// trim from start (in place)
void Util::Ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
/*****************************************************************************/
// trim from end (in place)
void Util::Rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
/*****************************************************************************/
// trim from both ends (in place)
void Util::Trim(std::string &s)
{
    Ltrim(s);
    Rtrim(s);
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
    FILE* fp = nullptr;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == nullptr )
        return 0;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return 0;      /* Can't read? */
    }
    fclose( fp );
    return static_cast<std::int32_t>(rss * sysconf( _SC_PAGESIZE));

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
    return static_cast<std::int32_t>(rusage.ru_maxrss * 1024L);
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

        // Now the hex byte_to_hexcode for the specific character.
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

void Util::ByteToHex(const char byte, char *out)
{
    int i = 0U;
    static const char binHex[] = "0123456789ABCDEF";

    out[2*i] = binHex[(byte >> 4) & 0x0F];
    out[2*i + 1] = binHex[byte & 0x0F];
}


std::string Util::ToHex(const char *buf, size_t size)
{
    char out[2];
    std::string hexstr;

    for (size_t i = 0U; i < size; i++)
    {
        ByteToHex(buf[i], &out[0]);

        hexstr += out[0];
        hexstr += out[1];
    }

    return hexstr;
}

std::string Util::FileToString(const std::string &filePath)
{
    std::ifstream inFile(filePath, std::ifstream::in);

    std::stringstream ss;
    ss << inFile.rdbuf(); //read the file
    std::string str = ss.str(); //str holds the content of the file
    inFile.close();
    return str;
}

bool Util::StringToFile(const std::string &filePath, const std::string &data, bool makeExecutable)
{
    bool success = false;
    std::ofstream outFile(filePath, std::ifstream::out);

    if (outFile.is_open())
    {
        outFile << data << std::endl;
        outFile.close();
        success = true;
    }
#ifdef USE_LINUX_OS
    if (makeExecutable)
    {
        char mode[] = "0777";
        int i;
        i = strtol(mode, 0, 8);
        (void) chmod (filePath.c_str(), i);
    }
#else
    (void) makeExecutable;
#endif
    return success;
}

bool Util::IsDigitOrAlpha(const std::string &s)
{
    return std::any_of(s.begin(), s.end(), [](char c) {
        return isdigit(c) || isalpha(c);
    });
}


/* Returns the MAC Address
   Params: int iNetType - 0: ethernet, 1: Wifi
           char chMAC[6] - MAC Address in binary format
   Returns: 0: success
           -1: Failure
    */
int Util::GetMacAddress(const char *ifname, unsigned char chMAC[6])
{
#if defined(USE_LINUX_OS)
    struct ifreq ifr;
    int sock;

    sock = socket(AF_INET,SOCK_DGRAM,0);
    strcpy(ifr.ifr_name, ifname);
    ifr.ifr_addr.sa_family = AF_INET;
    if (ioctl( sock, SIOCGIFHWADDR, &ifr ) < 0) {
        return -1;
    }
    memcpy(chMAC, ifr.ifr_hwaddr.sa_data, 6);
    close(sock);
#elif  defined(USE_WINDOWS_OS)
    IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);                  // [in] size of receive data buffer
    if (dwStatus != ERROR_SUCCESS)
    {
        printf("GetAdaptersInfo failed. err=%d\n", GetLastError());
        return 0;
    }

    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to  current adapter info
    do
    {
        for (int i = 0; i < 6; i++)
        {
            chMAC[i] = pAdapterInfo->Address[i];
        }
        pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
    } while (pAdapterInfo);

#elif  defined(USE_APPLE_OS)
    ifaddrs* iflist;
    bool found = false;
    if (getifaddrs(&iflist) == 0) {
        for (ifaddrs* cur = iflist; cur; cur = cur->ifa_next) {
            if ((cur->ifa_addr->sa_family == AF_LINK) &&
                    (strcmp(cur->ifa_name, ifname) == 0) &&
                    cur->ifa_addr) {
                sockaddr_dl* sdl = (sockaddr_dl*)cur->ifa_addr;
                memcpy(chMAC, LLADDR(sdl), sdl->sdl_alen);
                found = true;
                break;
            }
        }

        freeifaddrs(iflist);
    }
    if (!found) {
        return -1;
    }

#else
#error "PORTABLE CODE NEEDED HERE"

#endif

    return 0;
}

std::string Util::GetIpAddress(const char *ifname)
{
    std::string ip;
#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);

    int ret = ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    ip = inet_ntoa(reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr)->sin_addr);

    if (ret != 0)
    {
        ip = "Not connected";
    }
#elif defined(USE_WINDOWS_OS)

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;

    /* Variables used to return error message */
    LPVOID lpMsgBuf;

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the
        // necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
            ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(dwSize);

        }
        if (pIPAddrTable == NULL) {
          //  printf("Memory allocation failed for GetIpAddrTable\n");
            return ip;
        }
    }
    // Make a second call to GetIpAddrTable to get the
    // actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
      //  printf("GetIpAddrTable failed with error %d\n", dwRetVal);
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
            (LPTSTR)&lpMsgBuf, 0, NULL)) {
         //   printf("\tError: %s", lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        return ip;
     //   exit(1);
    }

   // printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);
    for (DWORD   i = 0; i < pIPAddrTable->dwNumEntries; i++) {

        IPAddr.S_un.S_addr = (u_long)(pIPAddrTable->table[i].dwAddr);
        ip = inet_ntoa(IPAddr);

        /*
        printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
        printf("\tIP Address[%d]:     \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwMask;
        printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwBCastAddr;
        printf("\tBroadCast[%d]:      \t%s (%ld%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
        printf("\tReassembly size[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwReasmSize);
        printf("\tType and State[%d]:", i);
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
            printf("\tPrimary IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
            printf("\tDynamic IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
            printf("\tAddress is on disconnected interface");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
            printf("\tAddress is being deleted");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
            printf("\tTransient address");
        printf("\n");
        */
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }
#else
#error "PORTABLE CODE NEEDED HERE"
#endif
    return ip;
}

std::vector<std::string> Util::GetDirectoryFiles(const std::string &dir, const std::string &extension)
{
    std::vector<std::string> files;

    if (std::filesystem::exists(dir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(dir))
        {
            const auto filenameStr = entry.path().filename().string();
            if (entry.is_regular_file())
            {
                if (GetFileExtension(filenameStr) == extension)
                {
                    files.push_back(filenameStr);
                }
            }
        }
    }
    return files;
}


std::vector<std::string> Util::ListFiles(const std::string &dir, const std::string &expression)
{
    std::vector<std::string> r;
    try
    {
        for(auto& p : std::filesystem::directory_iterator(dir))
        {
           if (p.is_regular_file()) //we eliminate directories in a list
           {
               std::string filename = p.path().filename().string();
               if (std::regex_match (filename, std::regex(expression) ))
               {
                    r.push_back(filename);
               }
           }
           else
           {
               continue;
           }
        }
    }
    catch(const std::filesystem::filesystem_error &e)
    {
        (void) e;
    }
    return r;
}


//=============================================================================
// End of file Util.cpp
//=============================================================================
