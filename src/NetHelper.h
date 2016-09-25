#ifndef NETHELPER_H
#define NETHELPER_H

#include "JsonValue.h"
#include "Common.h"

namespace helper
{

bool FromJson(Tarot::Distribution &dist, const JsonObject &obj)
{
    bool ret = true;

    dist.mFile = obj.GetValue("file").GetString();
    dist.mSeed = obj.GetValue("seed").GetInteger();
    dist.TypeFromString(obj.GetValue("type").GetString());

    return ret;
}

void ToJson(const Tarot::Distribution &dist, JsonObject &obj)
{
    obj.AddValue("file", dist.mFile);
    obj.AddValue("seed", dist.mSeed);
    obj.AddValue("type", dist.TypeToString());
}

}


#endif // NETHELPER_H
