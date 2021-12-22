#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#include <string>
#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
typedef void* LIB_HANDLE;

#elif defined(USE_WINDOWS_OS)
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

#endif // SHARED_LIBRARY_H
