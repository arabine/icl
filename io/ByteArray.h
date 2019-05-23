/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <vector>
#include <cstdint>
#include <iostream>

/*****************************************************************************/
/**
 * @brief The ByteArray class
 * Little endian
 *
 */
class ByteArray
{

public:

    // Constructors
    ByteArray(const char *data);
    ByteArray(const char *data, uint32_t size);
    ByteArray(const std::string &data);
    ByteArray();

    // Getters
    std::uint16_t GetUint16(std::uint32_t index) const;
    std::uint32_t GetUint32(std::uint32_t index) const;
    std::string ToSring() const;
    std::uint32_t Size() const;
    std::uint8_t Get(std::uint32_t index) const;

    // Setters
    void PushBack(std::uint8_t byte);
    void Put(std::uint32_t index, std::uint8_t value);

    // Helpers
    void Clear();
    ByteArray SubArray(std::uint32_t index, std::uint32_t len) const;
    void Erase(std::uint32_t index, std::uint32_t len);
    void Alloc(std::uint32_t size);
    std::uint8_t *Data();
    const char *Data() const;
    int32_t FindFirstOf(std::uint8_t item);

    // Operators
    bool operator == (const std::string &rhs) const;
    ByteArray &operator=(const ByteArray &rhs);
    ByteArray &operator += (const ByteArray &rhs);
    ByteArray operator+(const ByteArray &rhs) const;

private:
    std::vector<std::uint8_t> mData;

    void Append(const std::vector<std::uint8_t> &data);
};


#endif // BYTEARRAY_H

//=============================================================================
// End of file ByteArray.h
//=============================================================================
