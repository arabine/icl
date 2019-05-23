/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef BYTE_STREAM_WRITER_H
#define BYTE_STREAM_WRITER_H

#include <deque>
#include <cstdint>
#include <iostream>
#include "ByteArray.h"

/*****************************************************************************/
class ByteStreamWriter
{

public:
    ByteStreamWriter(ByteArray &array);

    /**
     * @brief Sets the write/read index to a specified position in the array
     * @param pos
     */
    void Seek(std::uint32_t pos);

    // Operators
    ByteStreamWriter &operator << (const std::uint8_t &d);
    ByteStreamWriter &operator << (const std::uint16_t &d);
    ByteStreamWriter &operator << (const std::uint32_t &d);
    ByteStreamWriter &operator << (const std::int32_t &d);
    ByteStreamWriter &operator << (const bool &d);
    /**
     * @brief Overloaded operator to add a string to a byte stream
     * The first uint32_t will contain the size of the string
     * Then the string bytes are streamed, without any null-termination
     * @param s
     * @return
     */
    ByteStreamWriter &operator << (const std::string &s);

private:
    ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTE_STREAM_WRITER_H

//=============================================================================
// End of file ByteStreamWriter.h
//=============================================================================
