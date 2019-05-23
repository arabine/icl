/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include "ByteArray.h"


/*****************************************************************************/
ByteArray::ByteArray(const char *data)
    : ByteArray(std::string(data))
{

}
/*****************************************************************************/
ByteArray::ByteArray(const std::string &data)
{
    for (std::uint32_t i = 0U; i < data.size(); i++)
    {
        mData.push_back(static_cast<std::uint8_t>(data[i]));
    }
}
/*****************************************************************************/
ByteArray::ByteArray(const char *data, std::uint32_t size)
{
    for (std::uint32_t i = 0U; i < size; i++)
    {
        mData.push_back(static_cast<std::uint8_t>(data[i]));
    }
}
/*****************************************************************************/
ByteArray::ByteArray()
{

}
/*****************************************************************************/
void ByteArray::PushBack(std::uint8_t byte)
{
    mData.push_back(byte);
}
/*****************************************************************************/
std::uint32_t ByteArray::Size() const
{
    return mData.size();
}
/*****************************************************************************/
void ByteArray::Clear()
{
    mData.clear();
}
/*****************************************************************************/
ByteArray ByteArray::SubArray(std::uint32_t index, std::uint32_t len) const
{
    ByteArray array;

    if ((index + len) <= mData.size())
    {
        array.mData.insert(array.mData.begin(), mData.begin() + index, mData.begin() + index + len);
    }
    return array;
}
/*****************************************************************************/
void ByteArray::Erase(std::uint32_t index, std::uint32_t len)
{
    if ((index + len) <= mData.size())
    {
        mData.erase(mData.begin() + index, mData.begin() + index + len);
    }
}
/*****************************************************************************/
void ByteArray::Alloc(uint32_t size)
{
    mData.resize(size);
}
/*****************************************************************************/
uint8_t *ByteArray::Data()
{
    return mData.data();
}
/*****************************************************************************/
const char *ByteArray::Data() const
{
    return reinterpret_cast<const char *>(mData.data());
}
/*****************************************************************************/
std::int32_t ByteArray::FindFirstOf(uint8_t item)
{
    std::int32_t ret = -1;
    std::vector<std::uint8_t>::iterator pos = std::find(mData.begin(), mData.end(), item);

    if (pos != mData.end())
    {
        ret = pos - mData.begin();
    }
    return ret;
}
/*****************************************************************************/
void ByteArray::Put(std::uint32_t index, std::uint8_t value)
{
    if (index < mData.size())
    {
        mData[index] = value;
    }
}
/*****************************************************************************/
std::uint8_t ByteArray::Get(std::uint32_t index) const
{
    std::uint8_t value = 0U;
    if (index < mData.size())
    {
        value = mData[index];
    }
    return value;
}
/*****************************************************************************/
bool ByteArray::operator ==(const std::string &rhs) const
{
    bool ret = true;

    if (mData.size() >= rhs.size())
    {
        for (std::uint32_t i = 0U; i < rhs.size(); i++)
        {
            if (rhs[i] != mData[i])
            {
                ret = false;
            }
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}
/*****************************************************************************/
ByteArray &ByteArray::operator=(const ByteArray &rhs)
{
    mData.clear();
    mData = rhs.mData;
    return *this;
}
/*****************************************************************************/
ByteArray &ByteArray::operator += (const ByteArray &rhs)
{
    *this = *this + rhs;
    return *this;
}
/*****************************************************************************/
ByteArray ByteArray::operator+(const ByteArray &rhs) const
{
    ByteArray result;
    result.Append(this->mData);
    result.Append(rhs.mData);
    return result;
}
/*****************************************************************************/
std::uint16_t ByteArray::GetUint16(std::uint32_t index) const
{
    std::uint16_t data = 0U;

    if (mData.size() >= sizeof(std::uint16_t))
    {
        if (index <= (mData.size() - sizeof(std::uint16_t)))
        {
            for (std::uint8_t i = 0U; i < 2U; i++)
            {
                std::uint16_t hword = mData[index + i];
                data = data + (hword << (i * 8U));
            }
        }
    }
    return data;
}
/*****************************************************************************/
std::uint32_t ByteArray::GetUint32(std::uint32_t index) const
{
    std::uint32_t data = 0U;

    if (mData.size() >= sizeof(std::uint32_t))
    {
        if (index <= (mData.size() - sizeof(std::uint32_t)))
        {
            for (std::uint8_t i = 0U; i < 4U; i++)
            {
                std::uint32_t word = mData[index + i];
                data = data + (word << (i * 8U));
            }
        }
    }
    return data;
}
/*****************************************************************************/
std::string ByteArray::ToSring() const
{
    std::string buf;

    for (std::uint32_t i = 0U; i < mData.size(); i++)
    {
        buf.push_back(static_cast<char>(mData[i]));
    }

    return buf;
}
/*****************************************************************************/
void ByteArray::Append(const std::vector<std::uint8_t> &data)
{
    std::vector<std::uint8_t>::iterator it = mData.end();
    mData.insert(it, data.begin(), data.end());
}

//=============================================================================
// End of file ByteArray.cpp
//=============================================================================
