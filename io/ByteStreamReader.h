/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef BYTE_STREAM_READER_H
#define BYTE_STREAM_READER_H

#include <deque>
#include <cstdint>
#include <sstream>
#include "ByteArray.h"

/*****************************************************************************/
class ByteStreamReader
{

public:
    ByteStreamReader(const ByteArray &array);

    /**
     * @brief Sets the write/read index to a specified position in the array
     * @param pos
     */
    void Seek(std::uint32_t pos);
    std::string ToString();

    // Operators
    ByteStreamReader &operator >> (std::uint8_t &d);
    ByteStreamReader &operator >> (std::uint16_t &d);
    ByteStreamReader &operator >> (std::uint32_t &d);
    ByteStreamReader &operator >> (std::int32_t &d);
    ByteStreamReader &operator >> (bool &d);
    ByteStreamReader &operator >> (std::string &s);

private:
    const ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTE_STREAM_READER_H

//=============================================================================
// End of file ByteStreamReader.h
//=============================================================================
