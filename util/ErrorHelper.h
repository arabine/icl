#ifndef ERRORHELPER_H
#define ERRORHELPER_H

#include <cstdint>
#include <string>
#include <map>
#include <iomanip>
#include "libutil.h"

struct ErrorHelper
{
    explicit ErrorHelper(uint8_t cat)
        : category(cat)
        , flags(0)
    {

    }

    uint8_t category;
    uint8_t flags;

    uint16_t GetErrorCode() const
    {
        uint16_t code = category;
        code <<= 8;
        code += flags;
        return (code);
    }

    std::string GetErrorHexString() const
    {
        std::stringstream stream;
        stream << std::hex << std::setfill ('0') << std::setw(sizeof(uint16_t)*2)  << GetErrorCode();
        return stream.str();
    }

    void Initialize()
    {
        flags = 0;
    }

    void SetBit(uint8_t bit) {
        flags = u8_set_bit(flags, bit);
    }

    void ClearBit(uint8_t bit) {
        flags = u8_clr_bit(flags, bit);
    }

    bool HasErrors() const
    {
        return (flags != 0);
    }

    // ids, signification
    std::map<uint8_t, std::string> errors;
};

#endif // ERRORHELPER_H
