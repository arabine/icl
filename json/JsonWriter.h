/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "JsonValue.h"

/*****************************************************************************/
class JsonWriter
{
public:
    static bool SaveToFile(const JsonObject &i_value, const std::string &fileName);
};


#endif // JSON_WRITER_H

//=============================================================================
// End of file JsonWriter.h
//=============================================================================
