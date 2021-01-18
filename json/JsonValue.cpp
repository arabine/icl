/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "JsonValue.h"
#include "Util.h"
#include <sstream>
#include <regex>

/*****************************************************************************/
// Helper function
static std::string CreateIndent(std::int32_t level)
{
    std::string indent;

    for (std::int32_t i = 0; i < (4 * level); i++)
    {
        indent += " ";
    }
    return indent;
}
/*****************************************************************************/
std::string JsonArray::ToString(std::int32_t level) const
{
    std::string crlf;
    std::string indent;
    std::int32_t nextLevel = -1;

    if (level >= 0)
    {
        crlf = "\n";
        indent = CreateIndent(level + 1);
        nextLevel = level + 1;
    }
    std::string text = "[" + crlf;

    std::uint32_t index = 0U;
    for (std::vector<JsonValue>::const_iterator iter = mArray.begin(); iter != mArray.end(); ++iter)
    {
        text += indent + iter->ToString(nextLevel);
        index++;
        if (index < mArray.size())
        {
            text += "," + crlf;
        }
    }

    text += crlf + indent + "]";
    return text;
}
/*****************************************************************************/
void JsonArray::Clear()
{
    mArray.clear();
}
/*****************************************************************************/
void JsonArray::AddValue(const JsonValue &value)
{
    mArray.push_back(value);
}
/*****************************************************************************/
bool JsonArray::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;
    std::uint32_t index = 0U;

    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::istringstream(keys[0]) >> index;
    keys.erase(keys.begin());

    if (index < Size())
    {
        if (keys.size() == 0)
        {
            mArray[index] = value;
            ret = true;
        }
        else
        {
            // Forward deeper
            ret = mArray[index].ReplaceValue(Util::Join(keys, ":"), value);
        }
    }

    return ret;
}
/*****************************************************************************/
bool JsonArray::DeleteEntry(uint32_t index)
{
    bool success = false;
    if (index < mArray.size())
    {
        mArray.erase(mArray.begin() + index);
        success = true;
    }
    return success;
}
/*****************************************************************************/
JsonValue JsonArray::GetEntry(std::uint32_t index) const
{
    JsonValue value;

    if (index < mArray.size())
    {
        value = mArray[index];
    }
    return value;
}

uint32_t JsonArray::Size() const
{
    return static_cast<std::uint32_t>(mArray.size());
}
/*****************************************************************************/

//          *                          *                                  *

/*****************************************************************************/
void JsonObject::Clear()
{
    mObject.clear();
}
/*****************************************************************************/
JsonObject::JsonObject(const JsonObject &obj)
{
    *this = obj;
}
/*****************************************************************************/
JsonObject &JsonObject::operator = (JsonObject const &rhs)
{
    mObject = rhs.mObject;
    return *this;
}
/*****************************************************************************/
std::string JsonObject::ToString(int32_t level) const
{
    std::string crlf;
    std::string indent;
    std::int32_t nextLevel = -1;

    if (level >= 0)
    {
        crlf = "\n";
        indent = CreateIndent(level + 1);
        nextLevel = level + 1;
    }

    std::string text = "{" + crlf;
    std::uint32_t index = 0U;

    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        text += indent + "\"" + it->first + "\":" + it->second.ToString(nextLevel);
        index++;
        if (index < mObject.size())
        {
            text += "," + crlf;
        }
    }

    text += crlf + indent + "}";
    return text;
}
/*****************************************************************************/
std::string JsonObject::ToCBor() const
{
    return "";
}
/*****************************************************************************/
void JsonObject::AddValue(const std::string &name, const JsonValue &value)
{
    mObject[name] = value;
}
/*****************************************************************************/
bool JsonObject::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;
    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::string name = keys[0];
    keys.erase(keys.begin());

    for (std::map<std::string, JsonValue>::iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == name)
        {
            if (keys.size() == 0)
            {
                // The key is in this object, replace it with the new value
                it->second = value;
                ret = true;
            }
            else
            {
                ret = it->second.ReplaceValue(Util::Join(keys, ":"), value); // go deeper
            }
        }
    }

    return ret;
}
/*****************************************************************************/
bool JsonObject::HasValue(const std::string &keyPath) const
{
    bool ret = false;
    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::string name = keys[0];
    keys.erase(keys.begin());

    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == name)
        {
            if (keys.size() == 0)
            {
                // The key is in this object
                ret = true;
            }
            else
            {
                ret = it->second.HasValue(Util::Join(keys, ":")); // go deeper
            }
        }
    }
    return ret;
}
/*****************************************************************************/
JsonValue JsonObject::GetValue(const std::string &keyPath) const
{
    JsonValue value;
    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::string name = keys[0];
    keys.erase(keys.begin());

    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == name)
        {
            if (keys.size() == 0)
            {
                // The key is in this object
                value = it->second;
            }
            else
            {
                value = it->second.FindValue(Util::Join(keys, ":")); // go deeper
            }
        }
    }

    return value;
}
/*****************************************************************************/
std::vector<std::string> JsonObject::GetKeys() const
{
    std::vector<std::string> keys;

    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        keys.push_back(it->first);
    }

    return keys;
}
/*****************************************************************************/

//          *                          *                                  *

/*****************************************************************************/
std::string JsonValue::ToString(std::int32_t level) const
{
    std::string text;

    if (IsString())
    {
        std::regex reg("\\\\");
        std::string escapedString = std::regex_replace(GetString(), reg, "\\\\");
        text = "\"" + escapedString + "\"";
    }
    else if (GetTag() == INTEGER)
    {
        text = std::to_string(GetInteger64());
    }
    else if (GetTag() == BOOLEAN)
    {
        if (GetBool())
        {
            text = "true";
        }
        else
        {
            text = "false";
        }
    }
    else if (GetTag() == DOUBLE)
    {
        text = std::to_string(GetDouble());
    }
    else if (GetTag() == NULL_VAL)
    {
        text = "null";
    }
    else if (IsObject())
    {
        text = mObject.ToString(level);
    }
    else
    {
        text = mArray.ToString(level);
    }

    return text;
}
/*****************************************************************************/
JsonValue::JsonValue(std::int32_t value)
    : mTag(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::uint32_t value)
    : mTag(INTEGER)
    , mIntegerValue(static_cast<std::int64_t>(value))
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::int64_t value)
    : mTag(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::uint16_t value)
    : mTag(INTEGER)
    , mIntegerValue(static_cast<std::int64_t>(value))
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::uint8_t value)
    : mTag(INTEGER)
    , mIntegerValue(static_cast<std::int64_t>(value))
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(double value)
    : mTag(DOUBLE)
    , mIntegerValue(0)
    , mDoubleValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const char *value)
    : mTag(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const std::string &value)
    : mTag(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(bool value)
    : mTag(BOOLEAN)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const JsonValue &value)
{
    *this = value;
}
/*****************************************************************************/
JsonValue::JsonValue(const JsonObject &obj)
    : mTag(OBJECT)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(false)
{
    mObject = obj;
}
/*****************************************************************************/
JsonValue::JsonValue(const JsonArray &array)
    : mTag(ARRAY)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(false)
{
    mArray = array;
}
/*****************************************************************************/
JsonValue::JsonValue()
    : mTag(INVALID)
    , mIntegerValue(0)
    , mDoubleValue(0.0)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue &JsonValue::operator =(const JsonValue &rhs)
{
    mTag = rhs.mTag;
    mObject = rhs.mObject;
    mArray = rhs.mArray;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}
/*****************************************************************************/
void JsonValue::Clear()
{
    mTag = INVALID;
    mObject.Clear();
    mArray.Clear();
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::string &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsString())
    {
        value = json.GetString();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::uint32_t &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = static_cast<std::uint32_t>(json.GetInteger());
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::uint16_t &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = static_cast<std::uint16_t>(json.GetInteger());
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::int32_t &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = json.GetInteger();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, bool &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsBoolean())
    {
        value = json.GetBool();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, double &value) const
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsDouble())
    {
        value = json.GetDouble();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::HasValue(const std::string &keyPath) const
{
    JsonValue val = FindValue(keyPath);
    return val.IsValid();
}
/*****************************************************************************/
JsonValue JsonValue::FindValue(const std::string &keyPath) const
{
    std::vector<std::string> keys = Util::Split(keyPath, ":");
    bool found = false;
    JsonValue temp = *this;

    for (std::uint32_t i = 0U; i < keys.size(); i++)
    {
        if (temp.IsObject())
        {
            if (temp.GetObj().HasValue(keys[i]))
            {
                temp = temp.GetObj().GetValue(keys[i]);
                found = true;
            }
        }
        else if (temp.IsArray())
        {
            for (JsonArray::iterator iter = temp.GetArray().begin(); iter != temp.GetArray().end(); ++iter)
            {
                if (iter->IsObject())
                {
                    if (iter->GetObj().HasValue(keys[i]))
                    {
                        temp = iter->GetObj().GetValue(keys[i]);
                        found = true;
                        break;
                    }
                }
            }
        }
        else
        {
            break;
        }
    }
    return found ? temp : JsonValue();
}
/*****************************************************************************/
bool JsonValue::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;

    if (IsObject())
    {
        ret = GetObj().ReplaceValue(keyPath, value);
    }
    else if (IsArray())
    {
        ret = GetArray().ReplaceValue(keyPath, value);
    }
    else
    {
        // Nothing
    }

    return ret;
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
