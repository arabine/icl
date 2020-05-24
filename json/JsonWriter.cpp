/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <fstream>
#include <iostream>
#include "JsonWriter.h"

/*****************************************************************************/
bool JsonWriter::SaveToFile(const JsonObject &i_value, const std::string &fileName)
{
    std::ofstream f;
    f.open(fileName, std::ios_base::out | std::ios_base::binary);

    if (f.is_open())
    {
        f << i_value.ToString(0U) << std::endl;
        f.close();
        return true;
    }
    return false;
}


//=============================================================================
// End of file JsonWriter.cpp
//=============================================================================
