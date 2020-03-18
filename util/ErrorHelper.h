#ifndef ERRORHELPER_H
#define ERRORHELPER_H

#include <cstdint>
#include <string>
#include <map>
#include "libutil.h"

struct ErrorHelper
{
    ErrorHelper()
        : category(0)
        , flags(0)
    {

    }

    uint8_t category;
    uint8_t flags;

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

    bool HasErrors() {
        return (flags != 0);
    }

    // ids, signification
    std::map<uint8_t, std::string> errors;
};

#endif // ERRORHELPER_H
