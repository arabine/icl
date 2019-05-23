/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef GETOPTIONS_H
#define GETOPTIONS_H

#include <algorithm>

class CommandLine
{

public:
    CommandLine(int argc, char *argv[])
        : mArgc(argc)
        , mArgv(argv)
        , mEnd(mArgv + mArgc)
    {

    }

    CommandLine()
        : mArgc(0)
        , mArgv(NULL)
        , mEnd(NULL)
    {

    }

    std::string GetOption(const std::string & option)
    {
        if (mArgc > 1)
        {
            char ** itr = std::find(mArgv, mEnd, option);
            if (itr != mEnd && ++itr != mEnd)
            {
                return *itr;
            }
        }
        return "";
    }

    std::string GetLastOption()
    {
        if (mArgc > 1)
        {
            return (mArgv[mArgc-1]);
        }
        return "";
    }

    bool Exists(const std::string& option)
    {
        bool exists = false;

        if (mArgc > 1)
        {
            exists = std::find(mArgv, mEnd, option) != mEnd;
        }
        return exists;
    }

private:
    int mArgc;
    char **mArgv;
    char **mEnd;
};

#endif // GETOPTIONS_H

//=============================================================================
// End of file GetOptions.h
//=============================================================================
