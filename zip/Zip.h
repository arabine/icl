/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef ZIP_H
#define ZIP_H

#include <cstdint>
#include <string>
#include <vector>


#include "miniz.h"

/*****************************************************************************/
class Zip
{
public:
    Zip();
    ~Zip();

    bool Open(const std::string &zip, bool isFile);
    std::uint32_t NumberOfFiles() { return mNumberOfFiles; }
    bool GetFile(const std::string &fileName, std::string &contents);
    std::vector<std::string> ListFiles();
    void Close();

    static int CompressBuffer(const char *input, size_t input_size, char *output);

private:
    mz_zip_archive mZipArchive;
    bool mIsValid;
    std::uint32_t mNumberOfFiles;
    std::vector<std::string> mFiles;
};

#endif // ZIP_H

//=============================================================================
// End of file Zip.cpp
//=============================================================================
