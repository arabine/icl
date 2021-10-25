#ifndef SHAREDLIBRARY_H
#define SHAREDLIBRARY_H

#include <string>
#ifdef USE_LINUX_OS
typedef void* LIB_HANDLE;
#else
#ifdef _MSC_VER
#include <Windows.h>
#else
// MINGW
#include <windows.h>
#endif

typedef HMODULE LIB_HANDLE;
#endif

class SharedLibrary
{
public:
    SharedLibrary();
    ~SharedLibrary();
    bool Open(const std::string &libraryName);

    void Close();
    bool Sym(const char *name, void **ptr);

    std::string GetErrorMessage() { return errmsg; }

private:
    LIB_HANDLE handle;
    std::string errmsg;
};

#endif // SHAREDLIBRARY_H
