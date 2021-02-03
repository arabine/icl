#include "SharedLibrary.h"
#include <dlfcn.h>
#include <errno.h>


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
    dlerror(); /* Reset error status. */
    errmsg.clear();
    handle = dlopen(libraryName.c_str(), RTLD_LAZY); // RTLD_NOW

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
        dlclose(handle);
        handle = nullptr;
    }
}


bool SharedLibrary::Sym(const char* name, void** ptr)
{
    bool success = false;
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

    return success;
}
