#include "SharedLibrary.h"

#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
    #include <dlfcn.h>
    #include <errno.h>
#elif defined(USE_WINDOWS_OS)

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
#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
    dlerror(); /* Reset error status. */
    errmsg.clear();
    handle = dlopen(libraryName.c_str(), RTLD_LAZY); // RTLD_NOW
#elif defined(USE_WINDOWS_OS)
    handle = LoadLibraryA(libraryName.c_str());
#endif

    if (handle != nullptr)
    {
        return true;
    }
    else
    {
#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
        const char* err = dlerror();
        if (err)
        {
            errmsg = std::string(err);
        }
#elif defined(USE_WINDOWS_OS)
        int loadLibraryError = GetLastError();
        errmsg = "LoadLibrary() error code: " + std::to_string(loadLibraryError) + " for file: " + libraryName;
#endif
    }

    return false;
}

void SharedLibrary::Close()
{
    errmsg.clear();
    if (handle != nullptr)
    {
        /* Ignore errors. No good way to signal them without leaking memory. */
#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
        dlclose(handle);
#elif defined(USE_WINDOWS_OS)
        FreeLibrary(handle);
#endif
        handle = nullptr;
    }
}


bool SharedLibrary::Sym(const char* name, void** ptr)
{
    bool success = false;

#if defined(USE_LINUX_OS) || defined(USE_APPLE_OS)
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

#elif defined(USE_WINDOWS_OS)
    *ptr = reinterpret_cast<void *>(GetProcAddress(handle, name));
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
