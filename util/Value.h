/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <cstdint>

/*****************************************************************************/
/**
 * @brief General purpose any Value
 * The currently supported values are:
 *  \li a double
 *  \li a string
 *  \li an integer
 *  \li a boolean
 *  \li a null value
 *
 * Examples of usage:
 *   - JSON base class
 *   - JavaScript C++ wrapper API
 *
 */
class Value
{
public:
    enum Type
    {
        INVALID,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING,
        NULL_VAL
    };

    Value(std::int32_t value);
    Value(std::int64_t value);
    Value(double value);
    Value(const char *value);
    Value(const std::string &value);
    Value(bool value);
    Value(const Value &value);
    Value(); // default constructor creates an invalid value!
    ~Value();

    Value &operator = (Value const &rhs);

    bool IsValid() const
    {
        return mType != INVALID;
    }

    bool IsNull() const
    {
        return mType == NULL_VAL;
    }

    Type GetType() const
    {
        return mType;
    }

    std::int32_t    GetInteger() const
    {
        return static_cast<int32_t>(mIntegerValue);
    }

    std::int64_t    GetInteger64() const
    {
        return mIntegerValue;
    }

    double          GetDouble() const
    {
        return mDoubleValue;
    }
    bool            GetBool() const
    {
        return mBoolValue;
    }
    std::string     GetString() const
    {
        return mStringValue;
    }

    bool IsJsonString() const
    {
        return mJsonString;
    }

    void SetJsonString(bool enable)
    {
        mJsonString = enable;
    }

    void SetNull()
    {
        mType = NULL_VAL;
    }

private:
    Type mType;
    std::int64_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;
    bool mJsonString;
};

#endif // VALUE_H

//=============================================================================
// End of file Value.h
//=============================================================================
