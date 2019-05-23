/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <deque>
#include <cstdint>
#include <sstream>
#include "ByteStreamReader.h"

/*****************************************************************************/
ByteStreamReader::ByteStreamReader(const ByteArray &array)
    : mArray(array)
    , mIndex(0U)
{

}
/*****************************************************************************/
/**
 * @brief Sets the write/read index to a specified position in the array
 * @param pos
 */
void ByteStreamReader::Seek(std::uint32_t pos)
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
std::string ByteStreamReader::ToString()
{
    std::stringstream array;
    array << "[";
    for (std::uint32_t i = 0; i < mArray.Size(); i++)
    {
        if (i != 0)
        {
            array << ", ";
        }
        array << std::hex << (std::uint32_t)mArray.Get(i);
    }
    array << "]";

    return array.str();
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint8_t &d)
{
    if (mIndex < mArray.Size())
    {
        d = mArray.Get(mIndex);
        mIndex++;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint16_t &d)
{
    std::uint8_t byte;
    std::uint16_t hword;

    d = 0U;
    if (mArray.Size() >= 2U)
    {
        for (std::uint8_t i = 0U; i < 2U; i++)
        {
            *this >> byte;
            hword = byte;
            d = d + (hword << (i * 8U));
        }
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint32_t &d)
{
    std::uint8_t byte;
    std::uint32_t word;

    d = 0U;
    if (mArray.Size() >= 4U)
    {
        for (std::uint8_t i = 0U; i < 4U; i++)
        {
            *this >> byte;
            word = byte;
            d = d + (word << (i * 8U));
        }
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::int32_t &d)
{
    std::uint32_t value;
    *this >> value;
    d = static_cast<std::int32_t>(value);
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (bool &d)
{
    std::uint8_t byte;

    *this >> byte;
    if (byte)
    {
        d = true;
    }
    else
    {
        d = false;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::string &s)
{
    std::uint32_t size;
    std::uint8_t byte;

    // The first 32-bit value indicates the size of the followed string
    *this >> size;

    // Robustness: the size must not be higher than the ByteArray remaining bytes
    // If the test fails, you have a bad synchronization between your
    // stream reader and your ByteArray
    std::uint32_t remainingBytes = mArray.Size() - mIndex;
    if (size <= remainingBytes)
    {
        for (std::uint32_t i = 0U; i < size; i++)
        {
            *this >> byte;
            s.push_back(static_cast<char>(byte));
        }
    }
    return *this;
}

//=============================================================================
// End of file ByteStreamReader.cpp
//=============================================================================
