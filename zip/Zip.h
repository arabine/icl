/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef ZIP_H
#define ZIP_H

#include <cstdint>
#include <string>
#include <vector>

#ifndef ZIP_CPP_INCLUDED
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"
#endif

/*****************************************************************************/
class Zip
{
public:
    Zip();
    ~Zip();

    bool Open(const std::string &zip, bool isFile);
    std::uint32_t NumberOfFiles() { return mNumberOfFiles; }
    bool GetFile(const std::string &fileName, std::string &contents);
    void Close();

    static int CompressBuffer(const char *input, size_t input_size, char *output);

private:
    mz_zip_archive mZipArchive;
    bool mIsValid;
    std::uint32_t mNumberOfFiles;
};

#endif // ZIP_H

//=============================================================================
// End of file Zip.cpp
//=============================================================================
