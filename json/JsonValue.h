/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef JSONVALUE_H
#define JSONVALUE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// Forward declarations to resolve inter-dependency between Array and Object
class JsonArray;
class JsonObject;
class JsonValue;

namespace CBor {

enum Major {
    UnsignedIntegerType = 0U,
    NegativeIntegerType = 1U,
    ByteStringType = 2U,
    TextStringType = 3U,
    ArrayType = 4U,
    MapType = 5U,           /* a.k.a. object */
    TagType = 6U,
    SimpleTypesType = 7U
};

}

/*****************************************************************************/
class JsonObject
{
public:
    JsonObject() {}
    JsonObject(const JsonObject &obj);

    std::string ToString(std::int32_t level = -1) const;
    std::string ToCBor() const;
    bool HasValue(const std::string &keyPath) const;
    JsonValue GetValue(const std::string &keyPath) const;
    void Clear();
    void AddValue(const std::string &name, const JsonValue &value);
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);
    std::uint32_t GetSize() { return static_cast<std::uint32_t>(mObject.size()); }
    std::vector<std::string> GetKeys() const;

    JsonObject &operator = (JsonObject const &rhs);

private:
    std::map<std::string, JsonValue> mObject;
};

/*****************************************************************************/
class JsonArray
{
public:
    std::string ToString(int32_t level = -1) const;
    void Clear();
    // JsonArray
    JsonValue GetEntry(std::uint32_t index) const;
    std::uint32_t Size() const;
    void AddValue(const JsonValue &value);
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);
    bool DeleteEntry(std::uint32_t index);

    typedef std::vector<JsonValue>::iterator iterator;
    iterator begin() { return mArray.begin(); }
    iterator end() { return mArray.end(); }

private:
    std::vector<JsonValue> mArray;
};
/*****************************************************************************/
class JsonValue
{
public:
    enum Tag
    {
        INVALID,
        ARRAY,
        OBJECT,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING,
        NULL_VAL
    };

    // From Value class
    JsonValue(std::int64_t value);
    JsonValue(std::int32_t value);
    JsonValue(std::uint32_t value);
    JsonValue(std::uint16_t value);
    JsonValue(std::uint8_t value);
    JsonValue(double value);
    JsonValue(const char *value);
    JsonValue(const std::string &value);
    JsonValue(bool value);
    JsonValue(const JsonValue &value);
    JsonValue(); // default constructor creates an invalid value!
    JsonValue(const JsonObject &obj);
    JsonValue(const JsonArray &array);

    // Implemented virtual methods from IJsonNode
    Tag GetTag() const
    {
        return mTag;
    }

    std::string ToString(int32_t level = -1) const;
    void Clear();

    JsonValue &operator = (JsonValue const &rhs);

    bool IsValid() const      { return mTag != INVALID; }
    bool IsArray() const      { return mTag == ARRAY; }
    bool IsObject() const     { return mTag == OBJECT; }
    bool IsNull() const       { return mTag == NULL_VAL; }
    bool IsString() const     { return mTag == STRING; }
    bool IsInteger() const    { return mTag == INTEGER; }
    bool IsBoolean() const    { return mTag == BOOLEAN; }
    bool IsDouble() const     { return mTag == DOUBLE; }

    JsonObject &GetObj() { return mObject; }
    JsonArray &GetArray() { return mArray; }

    std::int32_t    GetInteger() const   { return static_cast<int32_t>(mIntegerValue); }
    std::int64_t    GetInteger64() const { return mIntegerValue; }
    double          GetDouble() const    { return mDoubleValue; }
    bool            GetBool() const      { return mBoolValue; }
    std::string     GetString() const    { return mStringValue; }

    bool GetValue(const std::string &nodePath, std::string &value) const;
    bool GetValue(const std::string &nodePath, std::uint32_t &value) const;
    bool GetValue(const std::string &nodePath, std::uint16_t &value) const;
    bool GetValue(const std::string &nodePath, std::int32_t &value) const;
    bool GetValue(const std::string &nodePath, bool &value) const;
    bool GetValue(const std::string &nodePath, double &value) const;

    void SetNull()
    {
        mTag = NULL_VAL;
    }

    /**
     * @brief Recursively try to find a value using the key path separated by ':' character
     * @param keyPath
     * @return
     */
    bool HasValue(const std::string &keyPath) const;
    JsonValue FindValue(const std::string &keyPath) const;
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);

private:
    Tag mTag;
    JsonObject mObject;
    JsonArray mArray;

    //std::variant<std::int64_t, double, bool> mScaler;
    std::int64_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
