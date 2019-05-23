/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <algorithm>
#include <iostream>
#include "UniqueId.h"

/*****************************************************************************/
/**
 * @brief UniqueId::UniqueId
 *
 * 0 is a reserved id (not valid)
 * Max is strictly superior than min
 * The constructor will correct the input parameters according to these rules
 *
 * @param min
 * @param max
 */
UniqueId::UniqueId(std::uint32_t min, std::uint32_t max)
{
    if (min == 0U)
    {
        mMin = 1U;
    }
    else
    {
        mMin = min;
    }

    if (max <= mMin)
    {
        mMax = mMin + 1U;
    }
    else
    {
        mMax = max;
    }
}
/*****************************************************************************/
std::uint32_t UniqueId::TakeId()
{
    std::uint32_t id = cInvalidId;

    for (std::uint32_t i = mMin; i <= mMax; i++)
    {
        if (std::find(mUsedIds.begin(), mUsedIds.end(), i) == mUsedIds.end())
        {
            // Id not used
            id = i;
            mUsedIds.push_back(id);
            break;
        }
    }
    return id;
}
/*****************************************************************************/
uint32_t UniqueId::FindId()
{
    std::uint32_t id = 0U;

    for (std::uint32_t i = mMin; i <= mMax; i++)
    {
        if (std::find(mUsedIds.begin(), mUsedIds.end(), i) == mUsedIds.end())
        {
            // Id not used
            id = i;
            break;
        }
    }
    return id;
}
/*****************************************************************************/
void UniqueId::AddId(uint32_t id)
{
    if (!IsTaken(id))
    {
        mUsedIds.push_back(id);
    }
}
/*****************************************************************************/
// Return true if the id has been found AND successfully erased
bool UniqueId::ReleaseId(std::uint32_t id)
{
    bool ret = false;

    if (std::find(mUsedIds.begin(), mUsedIds.end(), id) != mUsedIds.end())
    {
        mUsedIds.erase(std::find(mUsedIds.begin(), mUsedIds.end(), id));
        ret = (std::find(mUsedIds.begin(), mUsedIds.end(), id) == mUsedIds.end());
    }
    return ret;
}
/*****************************************************************************/
bool UniqueId::IsTaken(std::uint32_t id)
{
    return (std::find(mUsedIds.begin(), mUsedIds.end(), id) != mUsedIds.end());
}
/*****************************************************************************/
void UniqueId::Dump()
{
    for (auto id: mUsedIds)
    {
        std::cout << id <<", ";
    }
    std::cout << std::endl;
}

//=============================================================================
// End of file UniqueId.cpp
//=============================================================================
