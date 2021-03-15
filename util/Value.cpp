/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "Value.h"

/*****************************************************************************/
Value::Value(int32_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(int64_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(double value)
    : mType(DOUBLE)
    , mIntegerValue(static_cast<std::int32_t>(value))
    , mDoubleValue(value)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(const char *value)
    : mType(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mStringValue(value)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(const std::string &value)
    : mType(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mStringValue(value)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(bool value)
    : mType(BOOLEAN)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(value)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::Value(const Value &value)
{
    *this = value;
}
/*****************************************************************************/
Value::Value()
    : mType(INVALID)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(false)
    , mJsonString(false)
{

}
/*****************************************************************************/
Value::~Value()
{

}
/*****************************************************************************/
Value &Value::operator =(const Value &rhs)
{
    mType = rhs.mType;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    mJsonString = rhs.mJsonString;
    return *this;
}

//=============================================================================
// End of file Value.cpp
//=============================================================================
