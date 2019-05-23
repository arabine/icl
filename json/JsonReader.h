/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef JSON_READER_H
#define JSON_READER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "JsonValue.h"

/*****************************************************************************/

/**
 * @brief The JsonReader class
 *
 * A Json reader object will parse a Json file and create an in-memory version of the
 * file contents.
 *
 * Then, several getters are provided to retreive a Json value. To get a value, the node
 * path must be passed as a request. A node path is built using all the keys of the node
 * path separated by a semi-colon.
 *
 * Example: let's take the following Json file:
 *
 * { "test": "hello", "test2": { "name": "toto" } }
 *
 * To get the value of the key "name", the node path will be: "test2:name"
 *
 */
class JsonReader
{

public:

    enum ParseStatus
    {
        JSON_PARSE_OK,
        JSON_PARSE_BAD_NUMBER,
        JSON_PARSE_BAD_STRING,
        JSON_PARSE_BAD_IDENTIFIER,
        JSON_PARSE_STACK_UNDERFLOW,
        JSON_PARSE_MISMATCH_BRACKET,
        JSON_PARSE_UNEXPECTED_CHARACTER,
        JSON_PARSE_UNQUOTED_KEY,
        JSON_PARSE_BREAKING_BAD,
        JSON_PARSE_ALLOC_ERROR
    };

    // Helpers
    static bool ParseFile(JsonValue &json, const std::string &fileName);
    static bool ParseString(JsonValue &json, const std::string &data);

private:
    static JsonReader::ParseStatus Parse(char *s, char **endptr, JsonValue &json);
    static JsonValue StringToNumber(char *s, char **endptr);

    /*****************************************************************************/
    static inline bool IsDelim(char c)
    {
        return isspace(c) || c == ',' || c == ':' || c == ']' || c == '}' || c == '\0';
    }
    /*****************************************************************************/
    static inline int CharToInt(char c)
    {
        if (c >= 'a')
        {
            return c - 'a' + 10;
        }
        if (c >= 'A')
        {
            return c - 'A' + 10;
        }
        return c - '0';
    }

};

#endif // JSON_READER_H

//=============================================================================
// End of file JsonReader.h
//=============================================================================
