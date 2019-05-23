/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <cstdint>
#include <list>
#include <mutex>

/*****************************************************************************/
/**
 * @brief The UniqueId class
 *
 * Manage a unique list of IDs.
 * Start at 1, 0 is considered as invalid
 *
 */
class UniqueId
{
public:
    static const std::uint32_t cInvalidId = 0U;

    UniqueId(std::uint32_t min, std::uint32_t max);

    std::uint32_t TakeId();
    std::uint32_t FindId();
    void AddId(std::uint32_t id);
    bool ReleaseId(std::uint32_t id);
    bool IsTaken(std::uint32_t id);
    std::uint32_t GetMin() { return mMin; }
    std::uint32_t GetMax() { return mMax; }
    void Clear() { mUsedIds.clear(); }
    void Dump();

private:
    std::uint32_t mMin;
    std::uint32_t mMax;
    std::list<std::uint32_t> mUsedIds;
};

#endif // UNIQUE_ID_H

//=============================================================================
// End of file UniqueId.h
//=============================================================================
