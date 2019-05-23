/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <deque>
#include <cstdint>
#include <iostream>
#include "ByteStreamWriter.h"

/*****************************************************************************/
ByteStreamWriter::ByteStreamWriter(ByteArray &array)
    : mArray(array)
    , mIndex(array.Size())
{

}
/*****************************************************************************/
/**
 * @brief Sets the write/read index to a specified position in the array
 * @param pos
 */
void ByteStreamWriter::Seek(std::uint32_t pos)
{
    if (pos < mArray.Size())
    {
        mIndex = pos;
    }
    else
    {
        mIndex = mArray.Size();
    }
}
/*****************************************************************************/
ByteStreamWriter &ByteStreamWriter::operator << (const std::uint8_t &d)
{
    if (mIndex >= mArray.Size())
    {
        // Append data at the end
        mArray.PushBack(d);
    }
    else
    {
        // Overwrite data
        mArray.Put(mIndex, d);
    }
    mIndex++;
    return *this;
}
/*****************************************************************************/
ByteStreamWriter &ByteStreamWriter::operator << (const std::uint16_t &d)
{
    std::uint8_t byte;
    std::uint16_t data = d;

    for (std::uint8_t i = 0U; i < 2U; i++)
    {
        byte = data & 0xFF;
        *this << byte;
        data = data >> 8;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamWriter &ByteStreamWriter::operator << (const std::uint32_t &d)
{
    std::uint8_t byte;
    std::uint32_t data = d;

    for (std::uint8_t i = 0U; i < 4U; i++)
    {
        byte = data & 0xFF;
        *this << byte;
        data = data >> 8;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamWriter &ByteStreamWriter::operator << (const std::int32_t &d)
{
    *this << static_cast<std::uint32_t>(d);
    return *this;
}
/*****************************************************************************/
ByteStreamWriter &ByteStreamWriter::operator << (const bool &d)
{
    std::uint8_t byte = 0U;

    if (d)
    {
        byte = 1U;
    }
    *this << byte;
    return *this;
}
/*****************************************************************************/
/**
 * @brief Overloaded operator to add a string to a byte stream
 * The first uint32_t will contain the size of the string
 * Then the string bytes are streamed, without any null-termination
 * @param s
 * @return
 */
ByteStreamWriter &ByteStreamWriter::operator << (const std::string &s)
{
    std::uint32_t size = s.size();
    std::uint8_t byte;

    *this << size;
    for (std::uint32_t i = 0U; i < size; i++)
    {
        byte = static_cast<std::uint8_t>(s.at(i));
        *this << byte;
    }
    return *this;
}

//=============================================================================
// End of file ByteStreamWriter.cpp
//=============================================================================
