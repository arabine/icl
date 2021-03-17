#include "SharedLibrary.h"

#ifdef USE_LINUX_OS
#include <dlfcn.h>
#include <errno.h>
#endif

#if USE_WINDOWS_OS

#endif

SharedLibrary::SharedLibrary()
    : handle(nullptr)
{

}

SharedLibrary::~SharedLibrary()
{
    Close();
}

bool SharedLibrary::Open(const std::string &libraryName)
{
#ifdef USE_LINUX_OS
    dlerror(); /* Reset error status. */
    errmsg.clear();
    handle = dlopen(libraryName.c_str(), RTLD_LAZY); // RTLD_NOW
#else

    handle = LoadLibraryA(libraryName.c_str());
#endif

    if (handle != nullptr)
    {
        return true;
    }

    return false;
}

void SharedLibrary::Close()
{
    errmsg.clear();
    if (handle != nullptr)
    {
        /* Ignore errors. No good way to signal them without leaking memory. */
#ifdef USE_LINUX_OS
        dlclose(handle);
#else
        FreeLibrary(handle);
#endif
        handle = nullptr;
    }
}


bool SharedLibrary::Sym(const char* name, void** ptr)
{
    bool success = false;

#ifdef USE_LINUX_OS
    dlerror(); /* Reset error status. */
    *ptr = dlsym(handle, name);
    const char* err = dlerror();
    if (err)
    {
        errmsg = std::string(err);
        success = false;
    }
    else
    {
        errmsg.clear();
        success = true;
    }

#else
    *ptr = GetProcAddress(handle, name);
    if (*ptr == nullptr)
    {
        char buf[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf, (sizeof(buf) / sizeof(buf[0])), NULL);
        errmsg.assign(buf);
    }
    else
    {
        errmsg.clear();
        success = true;
    }
#endif

    return success;
}
