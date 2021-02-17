#ifndef SHAREDLIBRARY_H
#define SHAREDLIBRARY_H

#include <string>

class SharedLibrary
{
public:
    SharedLibrary();
    ~SharedLibrary();
    bool Open(const std::string &libraryName);

    void Close();
    bool Sym(const char *name, void **ptr);
private:
    void* handle;
    std::string errmsg;
};

#endif // SHAREDLIBRARY_H
